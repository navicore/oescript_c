#!/usr/bin/perl
use Test::More tests => 43;
use oescript;
use DateTime;
use strict;
use warnings;

my $signals = oescript::OesSigHandler->new(file_name => 'mysigfile2.pid', appname=>"oesperl", loglvl=>"info");
ok($signals, 'should be signals impl');

my $dispatcher = oescript::OesDispatcher->new(is_async => 1, name => 'mydispatcher2');
ok($dispatcher, 'should be dispatcher impl');
$dispatcher->start();

#my $tdispatcher = oescript::OesThreadDispatcher->new(nthreads => 5);
my $tdispatcher = oescript::OesThreadDispatcher->new(nthreads => 1);
ok($tdispatcher, 'should be tdispatcher impl');
$tdispatcher->start();

my $store = oescript::OesStore->new(persist_level => 3, datadir => "data", is_async => 1);
ok($store, 'should be store impl');

my $kernel = oescript::OesKernel->new(dispatcher   => $dispatcher,
                                   tdispatcher  =>$tdispatcher,
                                   store        => $store);
ok($kernel, 'should be kernel impl');

#start oejson socket server
my $net = oescript::OesNet->new(dispatcher => $dispatcher);
ok($net, 'should be net impl');
my $json = oescript::OesServer->new(dispatcher  => $dispatcher,
                                    netspec     => "oejson://0.0.0.0:9555",
                                    net         => $net,
                                    store       => $store);
ok($json, 'should be json impl');
$json->addKernel("myspace", $kernel);

####################################
# hash write, then read, then take #
####################################

# hash write
my %hash = (key1 => "see_me",
         key2 => "see_me_too",
         key3 => undef);
my $lease = $kernel->write({dur => 5000, data_ref => \%hash});
printf "lid: %i\n", $lease->lid;
printf "lease now val: %s\n", DateTime->from_epoch( epoch => time() );
#my $exptime_txt = DateTime->from_epoch( epoch => $lease->exptime);
#printf "hash lease expiretime val: %s\n", $exptime_txt;
# hash read
my %hashtempl = (key1 => "see_me");
my $result_hash_ref = $kernel->read({template_ref => \%hashtempl, timeout => 9000})->();
ok($result_hash_ref, 'should not be null');
isnt($result_hash_ref->{key1}, 'see_you', 'should not be equal');
is($result_hash_ref->{key1}, 'see_me', 'should be equal');
is($result_hash_ref->{key2}, 'see_me_too', 'should be equal');

# hash take
%hashtempl = (key1 => "see_me", key3 => undef);
$result_hash_ref = $kernel->take({template_ref => \%hashtempl, timeout => 9000})->();
ok($result_hash_ref, 'should not be null');
isnt($result_hash_ref->{key1}, 'see_you', 'should not be equal');
is($result_hash_ref->{key1}, 'see_me', 'should be equal');
is($result_hash_ref->{key2}, 'see_me_too', 'should be equal');
ok(!defined $result_hash_ref->{key3}, 'should not be defined');

# should be gone now
%hashtempl = (key1 => "see_me");
$result_hash_ref = $kernel->read({template_ref => \%hashtempl, timeout => 0})->();
ok(!defined $result_hash_ref, 'should not be defined');

####################################
# list write, then read, then take #
####################################

# list write
my @list = ("see_me_list", "see_me_too2", "seemethree", "seemefour", 'seemefive', 'seeme6');
$lease = $kernel->write({dur => 7500, data_ref => \@list});
printf "lid: %i\n", $lease->lid;
printf "lease now val: %s\n", DateTime->from_epoch( epoch => time() );
#$exptime_txt = DateTime->from_epoch( epoch => $lease->exptime);
#printf "list lease expiretime val: %s\n", $exptime_txt;

# list read
my @listtempl = ('_', '_', '_', 'seemefour', '_', "seeme6");
my $result_list_ref = $kernel->read({template_ref => \@listtempl, timeout => 9000})->();
ok($result_list_ref, 'should not be null');
isnt($result_list_ref->[0], 'see_you', 'should not be equal');
is($result_list_ref->[0], 'see_me_list', 'should be equal');
is($result_list_ref->[5], 'seeme6', 'should be equal');

# list bad read
#extra wildcard at the end should break matching
@listtempl = ('_', '_', '_', 'seemefour', '_', "seeme6", '_');
my $f = $kernel->read({template_ref => \@listtempl, timeout => 100});
$result_list_ref = $f->();
ok(! $result_list_ref, 'should be null');

# async list read
@listtempl = ('_', 'see_me_too2', '_', '_', '_', "_");
$f = $kernel->read({template_ref => \@listtempl, timeout => 9000});
ok($f, 'should not be null');
my $new_result_list_ref = $f->();
isnt($new_result_list_ref->[0], 'see_you', 'should not be equal');
is($new_result_list_ref->[0], 'see_me_list', 'should be equal');
is($new_result_list_ref->[5], 'seeme6', 'should be equal');

# list take
@listtempl = ('see_me_list', '_', '_', '_', '_', "_");
$result_list_ref = $kernel->take({template_ref => \@listtempl, timeout => 9000})->();
ok($result_list_ref, 'should not be null');
isnt($result_list_ref->[0], 'see_you', 'should not be equal');
is($result_list_ref->[0], 'see_me_list', 'should be equal');
is($result_list_ref->[3], 'seemefour', 'should be equal');
is($result_list_ref->[5], 'seeme6', 'should be equal');

# should be gone now
@listtempl = ('see_me_list', '_', '_', '_', '_', "_");
$result_list_ref = $kernel->read({template_ref => \@listtempl, timeout => 100})->();
ok(! $result_list_ref, 'should be null');

##############################
# now do the same with a txn #
##############################

# start new txn
my $txn = $kernel->begin({dur => 10000});
ok($txn, 'should not be null');
is($txn->status, 'A');
printf "tid: %i\n", $txn->tid;
printf "           now val: %s\n", DateTime->from_epoch( epoch => time() );
#$exptime_txt = DateTime->from_epoch( epoch => $txn->exptime);
#printf "txn expiretime val: %s\n", $exptime_txt;

# write list 
#@list = ("txn_see_me_list", "txn_see_me_too2", "txn_seemethree", "txn_seemefour", 'txn_seemefive', 'txn_seeme6');
my @insidelist = ('one', 'two', 'three');
@list = ("txn_see_me_list", "txn_see_me_too2", "txn_seemethree", "txn_seemefour", 'txn_seemefive', 'txn_seeme6', \@insidelist);
$lease = $kernel->write({dur => 7500, data_ref => \@list});

# try to read an uncommitted txn. shouldn't be able to.
@listtempl = ('_', '_', '_', 'txn_seemefour', '_', "txn_seeme6");
$result_list_ref = $kernel->read({template_ref => \@listtempl, timeout => 100})->();
ok(! $result_list_ref, 'should be null');

# now commit it
printf("txn b4 status: %s\n", $txn->status);
is($txn->status, 'A');
$txn->commit();
printf("txn after status: %s\n", $txn->status);
is($txn->status, 'C');
#$exptime_txt = DateTime->from_epoch( epoch => $txn->exptime);
#my $now_txt = DateTime->from_epoch( epoch => time());
#printf "                now time val: %s\n", $now_txt;
#printf "committed txn expiretime val: %s\n", $exptime_txt;

# try to read a committed txn. should be able to.
@listtempl = ('_', '_', '_', 'txn_seemefour', '_', "txn_seeme6", '_');
$result_list_ref = $kernel->read({template_ref => \@listtempl, timeout => 100})->();
ok($result_list_ref, 'should not be null');
isnt($result_list_ref, "see_you");
is($result_list_ref->[0], "txn_see_me_list");
is($result_list_ref->[3], "txn_seemefour");
is($result_list_ref->[5], "txn_seeme6");

#  #cancel the lease
#printf "                now time val: %s\n", $now_txt;
#$exptime_txt = DateTime->from_epoch( epoch => $lease->exptime);
#printf "list b4 lease expiretime val: %s\n", $exptime_txt;
$lease->cancel();
#$exptime_txt = DateTime->from_epoch( epoch => $lease->exptime);
#printf "list af lease expiretime val: %s\n", $exptime_txt;

# try to read a cancelled lease. shouldn't be able to.
@listtempl = ('_', '_', '_', 'txn_seemefour', '_', "txn_seeme6");
$result_list_ref = $kernel->read({template_ref => \@listtempl, timeout => 100})->();
ok(! $result_list_ref, 'should be null');

#shutdown

print("cleaning up...\n");
#del kernel
#del store
print("quitting...\n");
#del dispatcher
#del tdispatcher
#del net
#del ynet
#del ysp
#del signals

