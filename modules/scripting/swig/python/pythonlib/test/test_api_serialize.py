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

#tdispatcher = OesThreadDispatcher(10)
tdispatcher = OesThreadDispatcher(1)
if tdispatcher == None: raise AsertionError
tdispatcher.start()

store = OesStore(3, "data", 1);
if store == None: raise AsertionError

kernel = OesKernel(dispatcher, tdispatcher, store, True, None, None, None, None)
if kernel == None: raise AsertionError

#start socket server
net = OesNet(dispatcher, None, None, None)
if net == None: raise AsertionError
json = OesServer(dispatcher, "oejson://0.0.0.0:2222", net, store)
if json == None: raise AsertionError
json.addKernel("myspace", kernel)

####################################
# DataObject                       #
####################################

tpl = (10, 20, 30)
bytes = pickle.dumps(tpl) #python2
#bytes = pickle.dumps(tpl, 0).decode("utf-8") #python3
#print("bytes: %s" % bytes)
nbytes = len(bytes)
slist = oescriptimpl.OesDataObject();
slist.set_nbytes(nbytes)
slist.set_bytes(bytes)

#bytes2 = slist.get_bytes()
bytes2 = slist.get_bytes().encode('utf-8')

tpl2 = pickle.loads(bytes2)
if tpl2[2] != 30:
    raise Exception( "bad" )

#####################################
# tuple write, then read, then take #
#####################################

kernel.writeData((91,92,93))

###########################################################################
###########################################################################
###########################################################################
# ejs there is something baaaddddddddddd                                  #
# increasing threads increases crashes.  only symptom is no bytes in read #
###########################################################################
###########################################################################
###########################################################################

r = kernel.readData((91,92,93), 50000)
print("result tuple len: %i" % len(r))
print("result tuple item len: %i" % len(r[0]))
print("result tuple item fld: %s" % r[0][0])
print("result tuple item fld: %s" % r[0][1])
print("result tuple item fld: %s" % r[0][2])

kernel.writeData({"pyone" : "py111", "pytwo" : 9222})
r = kernel.readData({"pyone" : "py111"}, 50000)
print("result dict len: %i" % len(r))
print("result dict item: %s" % r[0]["pytwo"])

####################################
# hash write, then read, then take #
####################################

# hash write
myhash = {"pykey1" : "see_me",
          "pykey2" : "see_me_too",
          "pykey3" : None}
lease = kernel.writeData(myhash)
print("dict lid: %i" % lease.lid)
print("now: %s" % datetime.datetime.now())

# hash read
templ = {"pykey1" : "see_me"}
result = kernel.readData(templ, timeout=0)
if result == None: raise AsertionError
if result[0] == None: raise AsertionError
print("sz: %i" % len(result))
if result[0]["pykey1"] != "see_me": raise AsertionError
if result[0]["pykey2"] != "see_me_too": raise AsertionError

# hash take
templ = {"pykey1" : "see_me", "pykey3" : None}
result = kernel.takeData(templ)
if result == None: raise AsertionError
if result[0] == None: raise AsertionError
if result[0]["pykey1"] != "see_me": raise AsertionError
if result[0]["pykey2"] != "see_me_too": raise AsertionError
if result[0]["pykey3"] != None: raise AsertionError

# should be gone now
templ = {"pykey1" : "see_me"}
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

#shutdown

