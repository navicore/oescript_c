#!/usr/bin/python

import datetime
import oescriptimpl
from oescript import *

print("now running python api tests: %s" % datetime.datetime.now())

loginmod = OesBuiltInLoginModule()
if loginmod == None: raise AsertionError
loginmod.addAccount("users", "Doghouse Reilly", "john bonham rocks")
failed = False
try:
    loginmod.addPrivilege("users", "{}")
except Exception as e:
    print("failing! %s" % e)
    failed = True

if failed == False: raise AsertionError

loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"connect"}')
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"disconnect"}')
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"put"}')
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"get"}')
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"start"}')
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"utxn"}')
loginmod.addPrivilege("myspace", "users", '{"t":"cmd", "n":"ulease"}')
failed = False
try:
    loginmod.addPrivilege("myspace", "users", '{"type":"cmd", "name":"upyours"}')
except Exception as e:
    print("more failing! %s" % e)
    failed = True

if failed == False: raise AsertionError

signals = None
signals = OesSigHandler("mysigfile2.pid", "oespy", "info")
if signals == None: raise AsertionError

dispatcher = OesDispatcher(1, "mydispatcher")
if dispatcher == None: raise AsertionError
dispatcher.start()

#tdispatcher = OesThreadDispatcher(4)
tdispatcher = OesThreadDispatcher(1)
if tdispatcher == None: raise AsertionError
tdispatcher.start()

store = OesStore(3, "data", 1);
if store == None: raise AsertionError

kernel = OesKernel(dispatcher, tdispatcher, store, False, None, None, None, None)
if kernel == None: raise AsertionError

#start socket server
net = OesNet(dispatcher, None, None, None)
if net == None: raise AsertionError
json = OesServer(dispatcher, "oejson://0.0.0.0:2222/myspace", net, store)
if json == None: raise AsertionError
json.addKernel("myspace", kernel)
json.addLoginModule(loginmod)

kernel.writeData((1,2,3))
kernel.writeData({"one" : "111", "two" : 222})

r = kernel.readData((1,2,3), 50000)
print("result tuple len: %i" % len(r))
print("result tuple item len: %i" % len(r[0]))
print("result tuple item fld: %s" % r[0][0])
print("result tuple item fld: %s" % r[0][1])
print("result tuple item fld: %s" % r[0][2])
r = kernel.readData({"one" : 111}, 50000)
print("result dict len: %i" % len(r))
print("result dict item: %s" % r[0]["two"])

####################################
# hash write, then read, then take #
####################################

# hash write
myhash = {"key1" : "see_me",
          "key2" : "see_me_too",
          "key3" : None}
lease = kernel.writeData(myhash)
print("dict lid: %i" % lease.lid)
print("now: %s" % datetime.datetime.now())

# hash read
templ = {"key1" : "see_me"}
result = kernel.readData(templ, timeout=0)
if result == None: raise AsertionError
if result[0] == None: raise AsertionError
print("sz: %i" % len(result))
if result[0]["key1"] != "see_me": raise AsertionError
if result[0]["key2"] != "see_me_too": raise AsertionError

# hash take
templ = {"key1" : "see_me", "key3" : None}
result = kernel.takeData(templ)
if result == None: raise AsertionError
if result[0] == None: raise AsertionError
if result[0]["key1"] != "see_me": raise AsertionError
if result[0]["key2"] != "see_me_too": raise AsertionError
if result[0]["key3"] != None: raise AsertionError

# should be gone now
templ = {"key1" : "see_me"}
result = kernel.readData(templ, timeout=0)
if result == None: raise AsertionError
if len(result) != 0: raise AsertionError

####################################
# list write, then read, then take #
####################################

# list write
mylist = ("see_me_list", "see_me_too2", "seemethree", "seemefour", 'seemefive', 'seeme6')

lease = kernel.writeData(dur = 7500, data = mylist )
print("list lid: %i" % lease.lid)

# list read
templ = ('_', '_', '_', 'seemefour', '_', "seeme6")
result = kernel.readData(templ, timeout=100)
if result == None: raise AsertionError
if result[0] == None: raise AsertionError
if result[0][0] != "see_me_list": raise AsertionError
if result[0][1] != "see_me_too2": raise AsertionError
if result[0][5] != "seeme6": raise AsertionError

# list bad read
#extra wildcard at the end should break matching
templ = ('_', '_', '_', 'seemefour', '_', "seeme6", '_')
result = kernel.readData(templ, timeout=100)
if len(result) != 0: raise AsertionError

# list take
templ = ('see_me_list', '_', '_', '_', '_', "_")
result = kernel.takeData(templ, timeout = 9000)
if result == None: raise AsertionError
if result[0] == None: raise AsertionError
if result[0][0] != "see_me_list": raise AsertionError
if result[0][1] != "see_me_too2": raise AsertionError
if result[0][3] != "seemefour": raise AsertionError
if result[0][5] != "seeme6": raise AsertionError

# should be gone now
templ = ('_', '_', '_', 'seemefour', '_', "seeme6")
result = kernel.readData(templ, timeout=100)
if len(result) != 0: raise AsertionError


##############################
# now do the same with a txn #
##############################

# start new txn
txn = kernel.begin(10000)
if txn == None: raise AsertionError
print("tid: %i\n" % txn.tid)
print("txn status: %c\n" % txn.status)
if txn.status != "A": raise AsertionError

print("        now: %s" % datetime.datetime.now())

# write list 
mylist = ("txn_see_me_list", "txn_see_me_too2", "txn_seemethree", "txn_seemefour", 'txn_seemefive', 'txn_seeme6')
lease = kernel.writeData(dur = 7500, data = mylist)
mylistX = ("one", "two", "three")
leaseX = kernel.writeData(dur = 7500, data = mylistX)
if lease.lid == leaseX.lid: raise AsertionError

# try to read an uncommitted txn. shouldn't be able to.
templ = ('_', '_', '_', 'txn_seemefour', '_', "txn_seeme6")
result = kernel.readData(templ, timeout = 100)
if len(result) != 0: raise AsertionError

# now commit it
print("txn b4 status: %c\n" % txn.status)
if txn.status != "A": raise AsertionError
txn.commit()
print("txn af status: %c\n" % txn.status)
if txn.status != "C": raise AsertionError
print("                  now: %s" % datetime.datetime.now())

# try to read a committed txn. should be able to.
templ = ('_', '_', '_', 'txn_seemefour', '_', "txn_seeme6")
result = kernel.readData(templ, timeout = 100)
if len(result) != 1: raise AsertionError
if result[0][5] != "txn_seeme6": raise AsertionError

#cancel the lease
lease.cancel()
print("cancelled lease lid: %s" % lease.lid)

# try to read a cancelled lease. shouldn't be able to.
templ = ('_', '_', '_', 'txn_seemefour', '_', "txn_seeme6")
result = kernel.readData(templ, timeout = 100)
if len(result) != 0: raise AsertionError

#############################################
## rollback take and read many times test: ##
#############################################
# write data (autocommit)
mylist = ("plrbtst_key", "plrbtst_value1", "plrbtst_value2")
lease = kernel.writeData(dur = 7500, data = mylist)
# try to read, assert ~nil 
templ = ('plrbtst_key', '_', '_')
result = kernel.readData(templ, timeout = 100)
if len(result) != 1: raise AsertionError
if result[0][2] != "plrbtst_value2": raise AsertionError
# begin txn
# start new txn
txn = kernel.begin(10000)
if txn == None: raise AsertionError
print("tid: %i\n" % txn.tid)
print("txn status: %c\n" % txn.status)
if txn.status != "A": raise AsertionError
# take
result = kernel.takeData(templ, timeout = 100)
if len(result) != 1: raise AsertionError
if result[0][2] != "plrbtst_value2": raise AsertionError
# try to read, assert nil 
result = kernel.readData(templ, timeout = 100)
if len(result) != 0: raise AsertionError
# rollback
txn.rollback()
print("tid: %i\n" % txn.tid)
print("txn status: %c\n" % txn.status)
if txn.status != "R": raise AsertionError
# try to read, assert ~nil 
result = kernel.readData(templ, timeout = 100)
if len(result) != 1: raise AsertionError
if result[0][2] != "plrbtst_value2": raise AsertionError
# try to read, assert ~nil 
result = kernel.readData(templ, timeout = 100)
if len(result) != 1: raise AsertionError
if result[0][2] != "plrbtst_value2": raise AsertionError
# try to take, assert ~nil 
result = kernel.takeData(templ, timeout = 100)
if len(result) != 1: raise AsertionError
if result[0][2] != "plrbtst_value2": raise AsertionError
# try to read, assert nil 
result = kernel.readData(templ, timeout = 100)
if len(result) != 0: raise AsertionError

#shutdown

