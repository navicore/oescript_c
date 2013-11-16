#!/usr/bin/perl
use oescriptimpl;
use strict;
use warnings;
use Test::More tests => 7;

my $signals = oescriptimpl::OesSigHandler->new("mysigfile.pid", "oesperl", "info");
ok($signals, 'should be signals impl');

my $dispatcher = oescriptimpl::OesDispatcher->new(1, "mydispatcher");
ok($dispatcher, 'should be dispatcher impl');
$dispatcher->start();
$signals->add($dispatcher);

#my $tdispatcher = oescriptimpl::OesThreadDispatcher->new(5);
my $tdispatcher = oescriptimpl::OesThreadDispatcher->new(1);
ok($tdispatcher, 'should be tdispatcher impl');
$tdispatcher->start();
$signals->add($tdispatcher);

my $store = oescriptimpl::OesStore->new(3, "data", 1);
ok($store, 'should be store impl');
$signals->add($store);

my $kernel = oescriptimpl::OesKernel->new($dispatcher, $tdispatcher, $store, undef, undef, undef, undef);
ok($kernel, 'should be kernel impl');
$signals->add($kernel); #ejs fixme bug

#start oejson socket server
my $jnet = oescriptimpl::OesNet->new($dispatcher, undef, undef, undef);
ok($jnet, 'should be jnet impl');
$signals->add($jnet);
my $jsp = oescriptimpl::OesServer->new($dispatcher, "oejson://0.0.0.0:5556", $jnet, $store);
ok($jsp, 'should be jsp impl');
$jsp->addKernel("myspace", $kernel);
$signals->add($jsp);

#shutdown
done_testing()

