#!/usr/bin/perl
use Test::More tests => 40;
use oescript;
use oescriptimpl;
use DateTime;
use strict;
use warnings;

my $username = "Doghouse Reilly";
my $pwd = "john bonham rocks";

my $loginmod = oescript::OesBuiltInLoginModule->new();
ok($loginmod);
my $rc = $loginmod->addAccount("users", $username, $pwd);
ok(!$rc);
$rc = $loginmod->addAccount("users", "Simon Smedley", "secret");
ok(!$rc);
#$rc = $loginmod->addPrivilege("myspace", "users", "{}");
#ok($rc);
$rc = $loginmod->addPrivilege("myspace", "users", '{"t":"cmd", "n":"connect"}');
ok(!$rc);
$rc = $loginmod->addPrivilege("myspace", "users", '{"t":"cmd", "n":"disconnect"}');
ok(!$rc);
$rc = $loginmod->addPrivilege("myspace", "users", '{"t":"cmd", "n":"get"}');
ok(!$rc);
$rc = $loginmod->addPrivilege("myspace", "users", '{"t":"cmd", "n":"put"}');
ok(!$rc);
$rc = $loginmod->addPrivilege("myspace", "users", '{"t":"cmd", "n":"start"}');
ok(!$rc);
$rc = $loginmod->addPrivilege("myspace", "users", '{"t":"cmd", "n":"utxn"}');
ok(!$rc);
$rc = $loginmod->addPrivilege("myspace", "users", '{"t":"cmd", "n":"ulease"}');
ok(!$rc);

my $signals = oescript::OesSigHandler->new(file_name => 'mysigfile2.pid', appname=>"oesperl", loglvl=>"info");
ok($signals);

my $dispatcher = oescript::OesDispatcher->new(is_async => 1, name => 'mydispatcher2');
ok($dispatcher);
$dispatcher->start();

#my $tdispatcher = oescript::OesThreadDispatcher->new(nthreads => 5);
my $tdispatcher = oescript::OesThreadDispatcher->new(nthreads => 1);
ok($tdispatcher);
$tdispatcher->start();

my $store = oescript::OesStore->new(persist_level => 3, datadir => "data", is_async => 1);
ok($store);

my $kernel = oescript::OesKernel->new(dispatcher    => $dispatcher,
                                   tdispatcher      => $tdispatcher,
                                   store            => $store);
ok($kernel);

#start oejcl socket server
my $jnet = oescript::OesNet->new(dispatcher => $dispatcher);
ok($jnet);
my $jcl = oescript::OesServer->new( dispatcher   => $dispatcher,
                                    netspec      => "oejcl://0.0.0.0:5558/myspace",
                                    net          => $jnet,
                                    store        => $store);
ok($jcl);
$jcl->addKernel("myspace", $kernel);
$jcl->addLoginModule($loginmod);

#start socket client
my $yclnet = oescript::OesNet->new(dispatcher => $dispatcher);
ok($yclnet);
my $kclient = oescript::OesKernel->new(netspec      => "oejcl://127.0.0.1:5558/myspace",
                                       dispatcher   => $dispatcher,
                                       net          => $yclnet,
                                       username     => $username,
                                       pwd          => $pwd);

####################################
# hash write, then read, then take #
####################################

# hash write
my %hash = (key1 => "see_me",
         key2 => "see_me_too",
         key3 => undef);
my $lease = $kclient->write({dur => 5000, data_ref => \%hash});
#printf "lid: %i\n", $lease->lid;
#printf "lease now val: %s\n", DateTime->from_epoch( epoch => time() );
#my $exptime_txt = DateTime->from_epoch( epoch => $lease->exptime);
#printf "hash lease expiretime val: %s\n", $exptime_txt;
# hash read
my %hashtempl = (key1 => "see_me");
my $result_hash_ref = $kclient->read({template_ref => \%hashtempl, timeout => 9000})->();
ok($result_hash_ref);
isnt($result_hash_ref->{key1} , "see_you");
is($result_hash_ref->{key1} , "see_me");
is($result_hash_ref->{key2} , "see_me_too");

# hash take
%hashtempl = (key1 => "see_me", key3 => undef);
$result_hash_ref = $kclient->take({template_ref => \%hashtempl, timeout => 9000})->();
ok($result_hash_ref);
isnt($result_hash_ref->{key1} , "see_you");
is($result_hash_ref->{key1} , "see_me");
is($result_hash_ref->{key2} , "see_me_too");
ok(!$result_hash_ref->{key3});

# should be gone now
%hashtempl = (key1 => "see_me");
$result_hash_ref = $kclient->read({template_ref => \%hashtempl, timeout => 0})->();
ok(!$result_hash_ref);

####################################
# list write, then read, then take #
####################################

# list write
my @list = ("see_me_list", "see_me_too2", "seemethree", "seemefour", 'seemefive', 'seeme6');
$lease = $kclient->write({dur => 7500, data_ref => \@list});
printf "lid: %i\n", $lease->lid;
#printf "lease now val: %s\n", DateTime->from_epoch( epoch => time() );
#$exptime_txt = DateTime->from_epoch( epoch => $lease->exptime);
#printf "list lease expiretime val: %s\n", $exptime_txt;

# ################################### ok tests until here ##########################
# ################################### ok tests until here ##########################
# ################################### ok tests until here ##########################
# ################################### ok tests until here ##########################
# ################################### ok tests until here ##########################

# list read
my @listtempl = ('_', '_', '_', 'seemefour', '_', "seeme6");
my $result_list_ref = $kclient->read({template_ref => \@listtempl, timeout => 9000})->();
ok($result_list_ref);
isnt($result_list_ref->[0], "see_you");
is($result_list_ref->[0], "see_me_list");
is($result_list_ref->[5], "seeme6");

# list bad read
#extra wildcard at the end should break matching
@listtempl = ('_', '_', '_', 'seemefour', '_', "seeme6", '_');
my $f = $kclient->read({template_ref => \@listtempl, timeout => 100});
$result_list_ref = $f->();
ok(!$result_list_ref);

# async list read
@listtempl = ('_', 'see_me_too2', '_', '_', '_', "_");
$f = $kclient->read({template_ref => \@listtempl, timeout => 9000});
ok($f);
my $new_result_list_ref = $f->();
#isnt($new_result_list_ref->[0] , "see_you");
#is($new_result_list_ref->[0] , "see_me_list");
#is($new_result_list_ref->[5] , "seeme6");

# list take
@listtempl = ('see_me_list', '_', '_', '_', '_', "_");
$result_list_ref = $kclient->take({template_ref => \@listtempl, timeout => 9000})->();
ok($result_list_ref);
#isnt($result_list_ref->[0] , "see_you");
#is($result_list_ref->[0] , "see_me_list");
#is($result_list_ref->[3] , "seemefour");
#is($result_list_ref->[5] , "seeme6");

# should be gone now
@listtempl = ('see_me_list', '_', '_', '_', '_', "_");
$result_list_ref = $kclient->read({template_ref => \@listtempl, timeout => 100})->();
ok(! $result_list_ref);

##############################
# now do the same with a txn #
##############################

# start new txn
my $txn = $kclient->begin({dur => 10000});
ok($txn);
#is($txn->status , "A");
#printf "tid: %i\n", $txn->tid;
#printf "           now val: %s\n", DateTime->from_epoch( epoch => time() );
#$exptime_txt = DateTime->from_epoch( epoch => $txn->exptime);
#printf "txn expiretime val: %s\n", $exptime_txt;

# write list 
#@list = ("txn_see_me_list", "txn_see_me_too2", "txn_seemethree", "txn_seemefour", 'txn_seemefive', 'txn_seeme6');
my @insidelist = ('one', 'two', 'three');
@list = ("txn_see_me_list", "txn_see_me_too2", "txn_seemethree", "txn_seemefour", 'txn_seemefive', 'txn_seeme6', \@insidelist);
$lease = $kclient->write({dur => 7500, data_ref => \@list});

# try to read an uncommitted txn. shouldn't be able to.
@listtempl = ('_', '_', '_', 'txn_seemefour', '_', "txn_seeme6");
$result_list_ref = $kclient->read({template_ref => \@listtempl, timeout => 100})->();
ok(! $result_list_ref);

# now commit it
#printf("txn b4 status: %s\n", $txn->status);
#is($txn->status , "A");
$txn->commit();
#printf("txn after status: %s\n", $txn->status);
#is($txn->status , "C");
#$exptime_txt = DateTime->from_epoch( epoch => $txn->exptime);
#my $now_txt = DateTime->from_epoch( epoch => time());
#printf "                now time val: %s\n", $now_txt;
#printf "committed txn expiretime val: %s\n", $exptime_txt;

# try to read a committed txn. should be able to.
@listtempl = ('_', '_', '_', 'txn_seemefour', '_', "txn_seeme6", '_');
$result_list_ref = $kclient->read({template_ref => \@listtempl, timeout => 100})->();
ok($result_list_ref);
#isnt($result_list_ref->[0] , "see_you");
#is($result_list_ref->[0] , "txn_see_me_list");
#is($result_list_ref->[3] , "txn_seemefour");
#is($result_list_ref->[5] , "txn_seeme6");

#  #cancel the lease
#printf "                now time val: %s\n", $now_txt;
#$exptime_txt = DateTime->from_epoch( epoch => $lease->exptime);
#printf "list b4 lease id: %s\n", $lease->lid;
#printf "list b4 lease expiretime val: %s\n", $exptime_txt;
$lease->cancel();
#$exptime_txt = DateTime->from_epoch( epoch => $lease->exptime);
#printf "list af lease expiretime val: %s\n", $exptime_txt;

# try to read a cancelled lease. shouldn't be able to.
@listtempl = ('_', '_', '_', 'txn_seemefour', '_', "txn_seeme6");
$result_list_ref = $kclient->read({template_ref => \@listtempl, timeout => 100})->();
ok(! $result_list_ref);

#shutdown

#print("cleaning up...\n");
#del kernel
#del store
#print("quitting...\n");
#del dispatcher
#del tdispatcher
#del net
#del ynet
#del signals

