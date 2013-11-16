#!/usr/bin/perl
use Test::More tests => 44;
use oescript;
use oescriptimpl;
use DateTime;
use strict;
use warnings;

my $signals = oescript::OesSigHandler->new(file_name => 'mysigfile2.pid', appname=>"oesperl", loglvl=>"info");
ok($signals, 'should not be null');

my $dispatcher = oescript::OesDispatcher->new(is_async => 1, name => 'mydispatcher2');
ok($dispatcher, 'should not be null');
$dispatcher->start();

#my $tdispatcher = oescript::OesThreadDispatcher->new(nthreads => 5);
my $tdispatcher = oescript::OesThreadDispatcher->new(nthreads => 1);
ok($tdispatcher, 'should not be null');
$tdispatcher->start();

my $store = oescript::OesStore->new(persist_level => 3, datadir => "data", is_async => 1);
ok($store, 'should not be null');

my $kernel = oescript::OesKernel->new(dispatcher => $dispatcher,
                                   tdispatcher   => $tdispatcher,
                                   store         => $store,
                                   serialize     => 1);
ok($kernel, 'should not be null');

#start oejson socket server
my $jnet = oescript::OesNet->new(dispatcher => $dispatcher);
ok($jnet, 'should not be null');
my $jsp = oescript::OesServer->new(dispatcher   => $dispatcher,
                                   netspec      => "oejson://0.0.0.0:5556",
                                   net          => $jnet,
                                   store        => $store);
ok($jsp, 'should not be null');
$jsp->addKernel("myspace", $kernel);

#client api
#stringlist
my $slist = oescriptimpl::OesDataObject->new();
ok($slist, 'should not be null');
$slist->add_attr("seri_see_me");
$slist->add_attr("seri_see_me_too");

is($slist->get_nattrs(), 2);
$slist->add_attr("seri_see_me_three");
$slist->add_attr("seri_see_me_four");

####################################
# hash write, then read, then take #
####################################

# hash write
my %hash = (key1 => "seri_see_me",
         key2 => "seri_see_me_too",
         key3 => undef);
my $lease = $kernel->write({dur => 5000, data_ref => \%hash});
printf "lid: %i\n", $lease->lid;
printf "lease now val: %s\n", DateTime->from_epoch( epoch => time() );
#my $exptime_txt = DateTime->from_epoch( epoch => $lease->exptime);
#printf "hash lease expiretime val: %s\n", $exptime_txt;
# hash read
my %hashtempl = (key1 => "seri_see_me");
my $result_hash_ref = $kernel->read({template_ref => \%hashtempl, timeout => 9000})->();
ok($result_hash_ref);
isnt($$result_hash_ref{key1}, "see_you");
is($$result_hash_ref{key1} , "seri_see_me");
is($$result_hash_ref{key2} , "seri_see_me_too");

# hash take
%hashtempl = (key1 => "seri_see_me", key3 => undef);
$result_hash_ref = $kernel->take({template_ref => \%hashtempl, timeout => 9000})->();
ok($result_hash_ref);
isnt( $result_hash_ref->{key1} , "see_you");
is($result_hash_ref->{key1} , "seri_see_me");
is($result_hash_ref->{key2} , "seri_see_me_too");

# should be gone now
%hashtempl = (key1 => "seri_see_me");
$result_hash_ref = $kernel->read({template_ref => \%hashtempl, timeout => 0})->();
ok(! $result_hash_ref );

####################################
# list write, then read, then take #
####################################

# list write
my @list = ("seri_see_me_list", "seri_see_me_too2", "seemethree", "seemefour", 'seemefive', 'seeme6');
$lease = $kernel->write({dur => 7500, data_ref => \@list});
printf "lid: %i\n", $lease->lid;
#printf "lease now val: %s\n", DateTime->from_epoch( epoch => time() );
#$exptime_txt = DateTime->from_epoch( epoch => $lease->exptime);
#printf "list lease expiretime val: %s\n", $exptime_txt;

# list read
my @listtempl = ('_', '_', '_', 'seemefour', '_', "seeme6");
my $result_list_ref = $kernel->read({template_ref => \@listtempl, timeout => 9000})->();
ok($result_list_ref);
isnt($result_list_ref->[0] , "see_you");
is($result_list_ref->[0] , "seri_see_me_list");
is($result_list_ref->[5] , "seeme6");

# list bad read
#extra wildcard at the end should break matching
@listtempl = ('_', '_', '_', 'seemefour', '_', "seeme6", '_');
$result_list_ref = $kernel->read({template_ref => \@listtempl, timeout => 100})->();
ok(! $result_list_ref);

# async list read
@listtempl = ('_', 'seri_see_me_too2', '_', '_', '_', "_");
my $f = $kernel->read({template_ref => \@listtempl, timeout => 9000});
ok($f);
my $new_result_list_ref = $f->();
isnt($new_result_list_ref->[0] , "see_you");
is($new_result_list_ref->[0] , "seri_see_me_list");
is($new_result_list_ref->[5] , "seeme6");

# list take
@listtempl = ('seri_see_me_list', '_', '_', '_', '_', "_");
$result_list_ref = $kernel->take({template_ref => \@listtempl, timeout => 9000})->();
ok($result_list_ref);
isnt($result_list_ref->[0] , "see_you");
is($result_list_ref->[0] , "seri_see_me_list");
is($result_list_ref->[3] , "seemefour");
is($result_list_ref->[5] , "seeme6");

# should be gone now
@listtempl = ('seri_see_me_list', '_', '_', '_', '_', "_");
$result_list_ref = $kernel->read({template_ref => \@listtempl, timeout => 100})->();
ok(! $result_list_ref);

##############################
# now do the same with a txn #
##############################

# start new txn
my $txn = $kernel->begin({dur => 10000});
ok($txn);
is($txn->status , "A");
printf "tid: %i\n", $txn->tid;
printf "           now val: %s\n", DateTime->from_epoch( epoch => time() );
#$exptime_txt = DateTime->from_epoch( epoch => $txn->exptime);
#printf "txn expiretime val: %s\n", $exptime_txt;

# write list 
#@list = ("txn_seri_see_me_list", "txn_seri_see_me_too2", "seri_txn_seemethree", "seri_txn_seemefour", 'seri_txn_seemefive', 'seri_txn_seeme6');
my @insidelist = ('one', 'two', 'three');
@list = ("txn_seri_see_me_list", "txn_seri_see_me_too2", "seri_txn_seemethree", "seri_txn_seemefour", 'seri_txn_seemefive', 'seri_txn_seeme6', \@insidelist);
$lease = $kernel->write({dur => 7500, data_ref => \@list});

# try to read an uncommitted txn. shouldn't be able to.
@listtempl = ('_', '_', '_', 'seri_txn_seemefour', '_', "seri_txn_seeme6");
$result_list_ref = $kernel->read({template_ref => \@listtempl, timeout => 100})->();
ok(! $result_list_ref);

# now commit it
printf("txn b4 status: %s\n", $txn->status);
is($txn->status , "A");
$txn->commit();
printf("txn after status: %s\n", $txn->status);
is($txn->status , "C");
#$exptime_txt = DateTime->from_epoch( epoch => $txn->exptime);
#my $now_txt = DateTime->from_epoch( epoch => time());
#printf "                now time val: %s\n", $now_txt;
#printf "committed txn expiretime val: %s\n", $exptime_txt;

# try to read a committed txn. should be able to.
@listtempl = ('_', '_', '_', 'seri_txn_seemefour', '_', "seri_txn_seeme6", '_');
$result_list_ref = $kernel->read({template_ref => \@listtempl, timeout => 100})->();
ok($result_list_ref);
isnt($result_list_ref->[0] , "see_you");
is($result_list_ref->[0] , "txn_seri_see_me_list");
is($result_list_ref->[3] , "seri_txn_seemefour");
is($result_list_ref->[5] , "seri_txn_seeme6");

#  #cancel the lease
#printf "                now time val: %s\n", $now_txt;
#$exptime_txt = DateTime->from_epoch( epoch => $lease->exptime);
#printf "list b4 lease expiretime val: %s\n", $exptime_txt;
$lease->cancel();
#$exptime_txt = DateTime->from_epoch( epoch => $lease->exptime);
#printf "list af lease expiretime val: %s\n", $exptime_txt;

# try to read a cancelled lease. shouldn't be able to.
@listtempl = ('_', '_', '_', 'seri_txn_seemefour', '_', "seri_txn_seeme6");
$result_list_ref = $kernel->read({template_ref => \@listtempl, timeout => 100})->();
ok(! $result_list_ref);

#shutdown

print("cleaning up...\n");
#del kernel
#del store
print("quitting...\n");
#del dispatcher
#del tdispatcher
#del net
#del ysp
#del signals

