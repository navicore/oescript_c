#!/usr/bin/php
<?php

include("oescriptimpl.php");
assert_options(ASSERT_ACTIVE, true);
assert_options(ASSERT_WARNING, true);
assert_options(ASSERT_BAIL, true);
assert_options(ASSERT_CALLBACK, 'assert_callback');

function assert_callback( $script, $line, $message ) {
    echo 'error: ', $script,': line', $line,":\n";
    exit(1);
}

$signals = new_OesSigHandler("mysigfile.txt", "oesphp", "info");
assert($signals);

$dispatcher = new_OesDispatcher(1, "mydispatcher");
assert($dispatcher);
OesDispatcher_start($dispatcher);

$tdispatcher = new_OesThreadDispatcher(5);
assert($tdispatcher);
OesThreadDispatcher_start($tdispatcher);

$store = new_OesStore(3, "data", 1);
assert($store);

$kernel = new_OesKernel($dispatcher, $tdispatcher, $store, null, null, null, null);
assert($kernel);

#start socket server
$net = new_OesNet($dispatcher, 0, null, null);
assert($net);
$json = new_OesServer($dispatcher, "oejson://0.0.0.0:9555", $net, $store);
assert($json);
OesServer_addKernel($json, "myspace", $kernel);

#shutdown

?>

