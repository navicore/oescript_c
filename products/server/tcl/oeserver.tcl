#!/usr/bin/perl
#  
# The contents of this file are subject to the Apache License
#  Version 2.0 (the "License"); you may not use this file except in
#  compliance with the License. You may obtain a copy of the License 
#  from the file named COPYING and from http://www.apache.org/licenses/.
#  
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#  
#  The Original Code is OeScript.
# 
#  The Initial Developer of the Original Code is OnExtent, LLC.
#  Portions created by OnExtent, LLC are Copyright (C) 2008-2009
#  OnExtent, LLC. All Rights Reserved.
#
#
# FIXME TODO ejs todo: this is still in perl....
#
#
use oescript;
use oescriptimpl;
use DateTime;
use strict;
use warnings;

printf "starting perl oescript server: %s\n", DateTime->from_epoch( epoch => time() );


my $signals = oescript::OesSigHandler->new(file_name => 'mysigfile2.txt');
die unless $signals ne "";

#todo: signals.add for each obj...

my $io_dispatcher = oescript::OesDispatcher->new(is_async => 0, name => 'io_dispatcher');
die unless $io_dispatcher ne "";

my $template_matcher = oescript::OesDispatcher->new(is_async => 1, name => 'template_matcher');
die unless $io_dispatcher ne "";
$template_matcher->start();


my $db_threader = oescript::OesThreadDispatcher->new(nthreads => 4);
die unless $db_threader ne "";
$db_threader->start();

my $store = oescript::OesStore->new(persist_level => 3, datadir => "__replace_me_with_data_dir__", is_async => 1);
die unless $store ne "";

my $kernel = oescript::OesKernel->new(dispatcher   => $template_matcher,
                                      tdispatcher  => $db_threader,
                                      store        => $store,
                                      0);
die unless $kernel ne "";

#start oesp socket server
my $net = oescript::OesNet->new(dispatcher => $io_dispatcher);
die unless $net ne "";
my $oesp = oescript::OesServer->new(kernel      => $kernel,
                                    dispatcher  => $io_dispatcher,
                                    netspec     => "oesp://0.0.0.0:7777",
                                    net         => $net,
                                    store       => $store);
die unless $oesp ne "";

#start oejson socket server
my $jnet = oescript::OesNet->new(dispatcher => $io_dispatcher);
die unless $jnet ne "";
my $jsp = oescript::OesServer->new(kernel       => $kernel,
                                   dispatcher   => $io_dispatcher,
                                   netspec      => "oejson://0.0.0.0:7778",
                                   net          => $jnet,
                                   store        => $store);
die unless $jsp ne "";

$io_dispatcher->start(); #blocking call.  server is running now

$signals->shutdown(); #if is isn't already

