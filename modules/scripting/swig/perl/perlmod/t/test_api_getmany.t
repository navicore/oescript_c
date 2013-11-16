#!/usr/bin/perl
use Test::More tests => 26;
use oescript;
use oescriptimpl;
use DateTime;
use strict;
use warnings;

#todo: rewrite this test to loop over an configured number of items

my $signals = oescript::OesSigHandler->new(file_name => 'mysigfile2.pid', appname=>"oesperl", loglvl=>"info");
ok($signals, 'should not be null');

my $dispatcher = oescript::OesDispatcher->new(is_async => 1,
                                           name => 'getmany_test_dispatcher');
ok($dispatcher, 'should not be null');
$dispatcher->start();

my $tdispatcher = oescript::OesThreadDispatcher->new(nthreads => 1);
ok($tdispatcher, 'should not be null');
$tdispatcher->start();

my $store = oescript::OesStore->new(persist_level => 3,
                                    datadir => "data",
                                    is_async => 0);
ok($store, 'should not be null');

my $kernel = oescript::OesKernel->new(dispatcher   => $dispatcher,
                                   tdispatcher  =>$tdispatcher,
                                   store        => $store);
ok($kernel, 'should not be null');

#client api

################
# test getmany #
################

# hash write
my %hash1 = (key1 => "batch_test", key2 => "bitem_1");
my %hash2 = (key1 => "batch_test", key2 => "bitem_2");
my %hash3 = (key1 => "batch_test", key2 => "bitem_3");
my %hash4 = (key1 => "batch_test", key2 => "bitem_4");
my %hash5 = (key1 => "batch_test", key2 => "bitem_5");
my $lease1 = $kernel->write({dur => 5000, data_ref => \%hash1});
printf "lid1: %i\n", $lease1->lid;
my $lease2 = $kernel->write({dur => 5000, data_ref => \%hash2});
printf "lid2: %i\n", $lease2->lid;
my $lease4 = $kernel->write({dur => 5000, data_ref => \%hash4});
printf "lid4: %i\n", $lease4->lid;
my $lease3 = $kernel->write({dur => 5000, data_ref => \%hash3});
printf "lid3: %i\n", $lease3->lid;
my $lease5 = $kernel->write({dur => 5000, data_ref => \%hash5});
printf "lid5: %i\n", $lease5->lid;

# hash read
my %hashtempl = (key1 => "batch_test");
my $iter = $kernel->read({template_ref => \%hashtempl, nresults => 10});
ok($iter, 'should not be null');
while (my $r = $iter->()) {
   is($r->{key1}, "batch_test");
   printf("got one %s\n", $r->{key2});
}

###########################
# test concurrent futures #
###########################

# do takes with long timeouts then write items then join the futures

print("calling take 5 times\n");
%hashtempl = (key2 => "item_5");
my $f_ref5 = $kernel->take({template_ref   => \%hashtempl,
                            timeout             => 100000});
ok($f_ref5, 'should not be null');

%hashtempl = (key2 => "item_1");
my $f_ref1 = $kernel->take({template_ref   => \%hashtempl,
                            timeout             => 100000});
ok($f_ref1, 'should not be null');

%hashtempl = (key2 => "item_2");
my $f_ref2 = $kernel->take({template_ref   => \%hashtempl,
                            timeout             => 100000});
ok($f_ref2, 'should not be null');

%hashtempl = (key2 => "item_3");
my $f_ref3 = $kernel->take({template_ref   => \%hashtempl,
                            timeout             => 100000});
ok($f_ref3, 'should not be null');

%hashtempl = (key2 => "item_4");
my $f_ref4 = $kernel->take({template_ref   => \%hashtempl,
                            timeout             => 100000});
ok($f_ref4, 'should not be null');

#sleep(2);

print("calling write 5 times\n");
# hash write
%hash1 = (key1 => "future_test", key2 => "item_1");
%hash2 = (key1 => "future_test", key2 => "item_2");
%hash3 = (key1 => "future_test", key2 => "item_3");
%hash4 = (key1 => "future_test", key2 => "item_4");
%hash5 = (key1 => "future_test", key2 => "item_5");
$lease1 = $kernel->write({data_ref => \%hash1});
printf "flid1: %i\n", $lease1->lid;
$lease3 = $kernel->write({data_ref => \%hash3});
printf "flid3: %i\n", $lease3->lid;
$lease4 = $kernel->write({data_ref => \%hash4});
printf "flid4: %i\n", $lease4->lid;
$lease5 = $kernel->write({data_ref => \%hash5});
printf "flid5: %i\n", $lease5->lid;
$lease2 = $kernel->write({data_ref => \%hash2});
printf "flid2: %i\n", $lease2->lid;

my $r1 = $f_ref1->();
my $r2 = $f_ref2->();
my $r3 = $f_ref3->();
my $r4 = $f_ref4->();
my $r5 = $f_ref5->();

print("unpacking 5 futures from the 5 takes above\n");
printf "async take 1 got: %s\n", $r1->{key2};
printf "async take 2 got: %s\n", $r2->{key2};
printf "async take 3 got: %s\n", $r3->{key2};
printf "async take 4 got: %s\n", $r4->{key2};
printf "async take 5 got: %s\n", $r5->{key2};

isnt($r1->{key2}, $r2->{key2});
isnt($r1->{key2}, $r3->{key2});
isnt($r1->{key2}, $r4->{key2});
isnt($r1->{key2}, $r5->{key2});

isnt($r2->{key2}, $r3->{key2});
isnt($r2->{key2}, $r4->{key2});
isnt($r2->{key2}, $r5->{key2});

isnt($r3->{key2}, $r4->{key2});
isnt($r3->{key2}, $r5->{key2});

isnt($r4->{key2}, $r5->{key2});

#shutdown
print("quitting...\n");

