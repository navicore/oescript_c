#!/usr/bin/python
import oescriptimpl
import datetime

# security api

loginmod = oescriptimpl.OesBuiltInLoginModule()
if loginmod == None: raise AsertionError

# rest of swig api

print("now running python swig tests: %s" % datetime.datetime.now())
signals = None
signals = oescriptimpl.OesSigHandler("mysigfile.pid", "oespy", "info")
if signals == None: raise AsertionError

dispatcher = oescriptimpl.OesDispatcher(1, "mydispatcher")
if dispatcher == None: raise AsertionError
dispatcher.start()
signals.add(dispatcher)

#tdispatcher = oescriptimpl.OesThreadDispatcher(4)
tdispatcher = oescriptimpl.OesThreadDispatcher(1)
if tdispatcher == None: raise AsertionError
tdispatcher.start()
signals.add(tdispatcher)

store = oescriptimpl.OesStore(3, "data", 1);
if store == None: raise AsertionError
signals.add(store)

kernel = oescriptimpl.OesKernel(dispatcher, tdispatcher, store, None, None, None, None)
if kernel == None: raise AsertionError
signals.add(kernel)

#start socket server
net = oescriptimpl.OesNet(dispatcher, None, None, None)
if net == None: raise AsertionError
json = oescriptimpl.OesServer(dispatcher, "oejson://0.0.0.0:6666", net, store)
if json == None: raise AsertionError
json.addKernel("myspace", kernel)

#client api
#stringlist
slist = oescriptimpl.OesDataObject()
if slist == None: raise AsertionError
slist.add_attr("see_me")
slist.add_attr("see_me_too")
iter = slist.iter()
if slist.get_nattrs() != 2: raise AsertionError
if iter.nextItem() != "see_me": raise AsertionError
if iter.nextItem() != "see_me_too": raise AsertionError
slist.add_attr("see_me_three")
slist.add_attr("see_me_four")

print("now: %s" % datetime.datetime.now())
lease = kernel.write(slist, 5000, 0)
print("lid: %i" % lease.lid())

#exptime = datetime.datetime.fromtimestamp(lease.exptime()//1000)

values = oescriptimpl.OesDataObjectList()
if values == None: raise AsertionError
templ = values.newDataObject()
if templ == None: raise AsertionError
#exit(0) # bug 
templ.add_attr("see_me")
templ.add_attr("see_me_too")
print("trying to read")
results = kernel.read(values, 9000, 0, 1, 1, 0)
if results == None: raise AsertionError
print(results.length())
if results.length() != 1: raise AsertionError
iter = results.iter()
entry = iter.nextItem()
if entry == None: raise AsertionError
iter = entry.iter()
if iter.nextItem() != "see_me": raise AsertionError
if iter.nextItem() != "see_me_too": raise AsertionError
iter.nextItem()
if  iter.nextItem() != "see_me_four": raise AsertionError

#now do the same with a txn
txn = kernel.begin(10000)
#exptime = datetime.datetime.fromtimestamp(txn.exptime()//1000)
print("txn: %i" % txn.tid())
slist = oescriptimpl.OesDataObject()
if slist == None: raise AsertionError
slist.add_attr("txn_see_me")
slist.add_attr("txn_see_me_too")
lease = kernel.write(slist, 5000, 0)
print("2nd lid: %i" % lease.lid())

#try to read an uncommitted txn. shouldn't be able to.
values = oescriptimpl.OesDataObjectList()
if values == None: raise AsertionError
templ = values.newDataObject()
if templ == None: raise AsertionError
templ.add_attr("txn_see_me")
templ.add_attr("txn_see_me_too")
print("trying to read")
results = kernel.read(values, 200, 0, 1, 1, 0)
#because results is a future, it can not be a nil
if results == None: raise AsertionError
if results.length() != 0: raise AsertionError

#now commit it
if txn.status() != oescriptimpl.ACTIVE: raise AsertionError
print("now: %s" % datetime.datetime.now())
#exptime = datetime.datetime.fromtimestamp(txn.exptime()//1000)
txn.commit()
#exptime = datetime.datetime.fromtimestamp(txn.exptime()//1000)
if txn.status() != oescriptimpl.COMMITTED: raise AsertionError

#try to read an committed txn. should be able to now.
results = kernel.read(values, 200, 0, 1, 1, 0)
if results == None: raise AsertionError
if results.length() != 1: raise AsertionError
iter = results.iter()
entry = iter.nextItem()
iter = entry.iter()
if entry == None: raise AsertionError
if iter.nextItem() != "txn_see_me": raise AsertionError
if iter.nextItem() != "txn_see_me_too": raise AsertionError

#cancel the lease
print("now: %s" % datetime.datetime.now())
#exptime = datetime.datetime.fromtimestamp(lease.exptime()//1000)
lease.cancel()
#exptime = datetime.datetime.fromtimestamp(lease.exptime()//1000)

#try to read a cancelled lease. shouldn't be able to.
values = oescriptimpl.OesDataObjectList()
if values == None: raise AsertionError
exit(0)
templ = values.newList()
if templ == None: raise AsertionError
templ.add("txn_see_me")
templ.add("txn_see_me_too")
print("trying to read")
results = kernel.read(values, 200, 0, 1, 1, 0)
#because results is a future, it can not be a nil
if results == None: raise AsertionError
if results.length() != 0: raise AsertionError

#shutdown

print("cleaning up...")
del kernel
del store
print("quitting...")
#del dispatcher
#del tdispatcher
#del net
#del ynet
#del ysp
#del signals

