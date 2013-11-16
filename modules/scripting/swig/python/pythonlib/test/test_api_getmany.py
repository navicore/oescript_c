#!/usr/bin/python

import datetime
import oescriptimpl
from oescript import *

print("now running python api tests: %s" % datetime.datetime.now())

signals = None
signals = OesSigHandler("mysigfile2.pid", "oespy", "info")
if signals == None: raise AsertionError

dispatcher = OesDispatcher(1, "mydispatcher")
if dispatcher == None: raise AsertionError
dispatcher.start()

#tdispatcher = OesThreadDispatcher(0)
tdispatcher = OesThreadDispatcher(1)
if tdispatcher == None: raise AsertionError
tdispatcher.start()

store = OesStore(3, "data", 0);
if store == None: raise AsertionError

kernel = OesKernel(dispatcher, tdispatcher, store, False, None, None, None, None)
if kernel == None: raise AsertionError

################
# test getmany #
################

# hash write
myhash1 = {"key1" : "batch_test", "key2" : "bitem_1"}
myhash2 = {"key1" : "batch_test", "key2" : "bitem_2"}
myhash3 = {"key1" : "batch_test", "key2" : "bitem_3"}
myhash4 = {"key1" : "batch_test", "key2" : "bitem_4"}
myhash5 = {"key1" : "batch_test", "key2" : "bitem_5"}
lease1 = kernel.writeData(dur = 5000, data = myhash1)
print("lid1: %i" % lease1.lid)
lease2 = kernel.writeData(dur = 5000, data = myhash2)
print("lid2: %i" % lease2.lid)
lease4 = kernel.writeData(dur = 5000, data = myhash4)
print("lid4: %i" % lease4.lid)
lease3 = kernel.writeData(dur = 5000, data = myhash3)
print("lid3: %i" % lease3.lid)
lease5 = kernel.writeData(dur = 5000, data = myhash5)
print("lid5: %i" % lease5.lid)

# hash read
templ = {"key1" : "batch_test"}
results = kernel.readData(templ, nresults = 10)
if results == None: raise AsertionError
if len(results) != 5: raise AsertionError

for r in results:
    if r["key1"] != "batch_test": raise AsertionError
    print("got one: %s" % r["key2"])

###########################
# test concurrent futures #
###########################

# do 3 identical takes with long timeouts then write 5 items then join the futures

print("calling take 5 times")

templ = {"key2" : "item_5"}
results5 = kernel.takeData(templ)

templ = {"key2" : "item_1"}
results1 = kernel.takeData(templ)

templ = {"key2" : "item_2"}
results2 = kernel.takeData(templ)

templ = {"key2" : "item_3"}
results3 = kernel.takeData(templ)

templ = {"key2" : "item_4"}
results4 = kernel.takeData(templ)

print("calling write 5 times")

# hash write
hash1 = {"key1" : "future_test", "key2" : "item_1"}
hash2 = {"key2" : "future_test", "key2" : "item_2"}
hash3 = {"key3" : "future_test", "key2" : "item_3"}
hash4 = {"key4" : "future_test", "key2" : "item_4"}
hash5 = {"key5" : "future_test", "key2" : "item_5"}
lease1 = kernel.writeData(hash1)
lease3 = kernel.writeData(hash3)
lease4 = kernel.writeData(hash4)
lease5 = kernel.writeData(hash5)
lease2 = kernel.writeData(hash2)

rh1 = results1[0]["key1"]
if rh1 != "future_test": raise AsertionError

rh2 = results1[0]["key1"]
if rh2 != "future_test": raise AsertionError

rh3 = results1[0]["key1"]
if rh3 != "future_test": raise AsertionError

rh4 = results1[0]["key1"]
if rh4 != "future_test": raise AsertionError

rh5 = results1[0]["key1"]
if rh5 != "future_test": raise AsertionError

if results1[0]["key2"] == results2[0]["key2"] : raise AsertionError
if results1[0]["key2"] == results3[0]["key2"] : raise AsertionError
if results1[0]["key2"] == results4[0]["key2"] : raise AsertionError
if results1[0]["key2"] == results5[0]["key2"] : raise AsertionError

if results2[0]["key2"] == results3[0]["key2"] : raise AsertionError
if results2[0]["key2"] == results4[0]["key2"] : raise AsertionError
if results2[0]["key2"] == results5[0]["key2"] : raise AsertionError

if results3[0]["key2"] == results4[0]["key2"] : raise AsertionError
if results3[0]["key2"] == results5[0]["key2"] : raise AsertionError

if results4[0]["key2"] == results5[0]["key2"] : raise AsertionError

#   #shutdown
print("quitting...")

