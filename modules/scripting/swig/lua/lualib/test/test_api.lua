#!/usr/bin/lua
require "oescript"

print("===> test signals")
signals = oescript.OesSigHandler:new("mysigfile2.pid", "oeslua", "info")
assert(signals ~= nil)

print("===> test dispatcher")
dispatcher = oescript.OesDispatcher:new(1, "mydispatcher")
assert(dispatcher ~= nil)
dispatcher:start()

print("===> test tdispatcher")
tdispatcher = oescript.OesThreadDispatcher:new(5)
assert(tdispatcher ~= nil)
tdispatcher:start()

store = oescript.OesStore:new(3, "data", true);
assert(store ~= nil)

kernel = oescript.OesKernel:new(dispatcher, tdispatcher, store)
assert(kernel ~= nil)

--start socket server
net = oescript.OesNet:new(dispatcher, false, nil, nil)
assert(net ~= nil)
json = oescript.OesServer:new(dispatcher, "oejson://0.0.0.0:6666", net, store)
assert(json ~= nil)
json:addKernel("myspace", kernel)

print("===> test shutdown")
--shutdown

