#!/usr/bin/ruby

require 'test/unit'
require 'oescript'

class TestRubyApiMod < Test::Unit::TestCase

  def test_runme
#   print "now running python api tests: %s" % datetime.datetime.now()

signals = nil
signals = Oescript::OesSigHandler.new("mysigfile2.pid")
assert_not_nil(signals)

dispatcher = Oescript::OesDispatcher.new(1, "mydispatcher")
assert_not_nil(dispatcher)
dispatcher.start()

tdispatcher = Oescript::OesThreadDispatcher.new(4)
assert_not_nil(tdispatcher)
tdispatcher.start()

store = Oescript::OesStore.new(3, "data", 1)
assert_not_nil(store)

kernel = Oescript::OesKernel.new(dispatcher, tdispatcher, store, false, nil, nil)
assert_not_nil(kernel)

#start oesp socket server
net = Oescript::OesNet.new(dispatcher)
assert_not_nil(net)
oesp = Oescript::OesServer.new(dispatcher, "oesp://0.0.0.0:2222", net, store)
assert_not_nil(oesp)
oesp.addKernel("myspace", kernel)

# array write
kernel.writeData(["1",2,3])
kernel.writeData({"one" => "111", "two" => 222})

#r = kernel.readData([1,2,3], 50000, 1)

#   print "result tuple len: %i" % len(r)
#   print "result tuple item len: %i" % len(r[0])
#   print "result tuple item fld: %s" % r[0][0]
#   print "result tuple item fld: %s" % r[0][1]
#   print "result tuple item fld: %s" % r[0][2]
#   r = kernel.readData({"one" : 111}, 50000)
#   print "result dict len: %i" % len(r)
#   print "result dict item: %s" % r[0]["two"]
#   
#   ####################################
#   # hash write, then read, then take #
#   ####################################
#   
# hash write
# myhash = {"key1" => "see_me", "key2" => "see_me_too", "key3" => nil}
# lease = kernel.writeData(myhash)

#   print "dict lid: %i" % lease.lid
#   print "now: %s" % datetime.datetime.now()
#   print "expdate: %s" % lease.exptime
#   
#   # hash read
#   templ = {"key1" : "see_me"}
#   result = kernel.readData(templ, timeout=0)
#   if result == None: raise AsertionError
#   if result[0] == None: raise AsertionError
#   print "sz: %i" % len(result)
#   if result[0]["key1"] != "see_me": raise AsertionError
#   if result[0]["key2"] != "see_me_too": raise AsertionError
#   
#   # hash take
#   templ = {"key1" : "see_me", "key3" : None}
#   result = kernel.takeData(templ)
#   if result == None: raise AsertionError
#   if result[0] == None: raise AsertionError
#   if result[0]["key1"] != "see_me": raise AsertionError
#   if result[0]["key2"] != "see_me_too": raise AsertionError
#   if result[0]["key3"] != None: raise AsertionError
#   
#   # should be gone now
#   templ = {"key1" : "see_me"}
#   result = kernel.readData(templ, timeout=0)
#   if result == None: raise AsertionError
#   if len(result) != 0: raise AsertionError
#   
#   ####################################
#   # list write, then read, then take #
#   ####################################
#   
#   # list write
#   mylist = ("see_me_list", "see_me_too2", "seemethree", "seemefour", 'seemefive', 'seeme6')
#   
#   lease = kernel.writeData(dur = 7500, data = mylist )
#   print "list lid: %i" % lease.lid
#   print "list expdate: %s" % lease.exptime
#   
#   # list read
#   templ = ('_', '_', '_', 'seemefour', '_', "seeme6")
#   result = kernel.readData(templ, timeout=100)
#   if result == None: raise AsertionError
#   if result[0] == None: raise AsertionError
#   if result[0][0] != "see_me_list": raise AsertionError
#   if result[0][1] != "see_me_too2": raise AsertionError
#   if result[0][5] != "seeme6": raise AsertionError
#   
#   # list bad read
#   #extra wildcard at the end should break matching
#   templ = ('_', '_', '_', 'seemefour', '_', "seeme6", '_')
#   result = kernel.readData(templ, timeout=100)
#   if len(result) != 0: raise AsertionError
#   
#   # list take
#   templ = ('see_me_list', '_', '_', '_', '_', "_")
#   result = kernel.takeData(templ, timeout = 9000)
#   if result == None: raise AsertionError
#   if result[0] == None: raise AsertionError
#   if result[0][0] != "see_me_list": raise AsertionError
#   if result[0][1] != "see_me_too2": raise AsertionError
#   if result[0][3] != "seemefour": raise AsertionError
#   if result[0][5] != "seeme6": raise AsertionError
#   
#   # should be gone now
#   templ = ('_', '_', '_', 'seemefour', '_', "seeme6")
#   result = kernel.readData(templ, timeout=100)
#   if len(result) != 0: raise AsertionError
#   
#   
#   ##############################
#   # now do the same with a txn #
#   ##############################
#   
#   # start new txn
#   txn = kernel.begin(10000)
#   if txn == None: raise AsertionError
#   print "tid: %i\n" % txn.tid
#   print "txn status: %c\n" % txn.status
#   if txn.status != "A": raise AsertionError
#   
#   print "        now: %s" % datetime.datetime.now()
#   print "txn exptime: %s" % txn.exptime
#   
#   # write list 
#   mylist = ("txn_see_me_list", "txn_see_me_too2", "txn_seemethree", "txn_seemefour", 'txn_seemefive', 'txn_seeme6')
#   lease = kernel.writeData(dur = 7500, data = mylist)
#   
#   # try to read an uncommitted txn. shouldn't be able to.
#   templ = ('_', '_', '_', 'txn_seemefour', '_', "txn_seeme6")
#   result = kernel.readData(templ, timeout = 100)
#   if len(result) != 0: raise AsertionError
#   
#   # now commit it
#   print "txn b4 status: %c\n" % txn.status
#   if txn.status != "A": raise AsertionError
#   txn.commit()
#   print "txn af status: %c\n" % txn.status
#   if txn.status != "C": raise AsertionError
#   print "                  now: %s" % datetime.datetime.now()
#   print "committed txn exptime: %s" % txn.exptime
#   
#   # try to read a committed txn. should be able to.
#   templ = ('_', '_', '_', 'txn_seemefour', '_', "txn_seeme6")
#   result = kernel.readData(templ, timeout = 100)
#   if len(result) != 1: raise AsertionError
#   if result[0][5] != "txn_seeme6": raise AsertionError
#   
#   #cancel the lease
#   print "     good lease exptime: %s" % lease.exptime
#   lease.cancel()
#   print "cancelled lease exptime: %s" % lease.exptime
#   
#   # try to read a cancelled lease. shouldn't be able to.
#   templ = ('_', '_', '_', 'txn_seemefour', '_', "txn_seeme6")
#   result = kernel.readData(templ, timeout = 100)
#   if len(result) != 0: raise AsertionError
#   
#   #shutdown
end
end
