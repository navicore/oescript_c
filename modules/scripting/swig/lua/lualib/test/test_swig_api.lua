#!/usr/bin/lua
require "oescriptimpl"

print("===> begin swig api tests")

signals = oescriptimpl.OesSigHandler("mysigfile.pid", "oespy", "info")
assert(signals ~= nil)

dispatcher = oescriptimpl.OesDispatcher(1, "mydispatcher")
assert(dispatcher ~= nil)
dispatcher:start()
signals:add(dispatcher)

tdispatcher = oescriptimpl.OesThreadDispatcher(5)
assert(tdispatcher ~= nil)
tdispatcher:start()
signals:add(tdispatcher)

store = oescriptimpl.OesStore(3, "data", true);
assert(store ~= nil)
signals:add(store)

kernel = oescriptimpl.OesKernel(dispatcher, tdispatcher, store, nil, nil, nil, nil)
assert(kernel ~= nil)
signals:add(kernel) --todo: fixme bug

--start socket server
net = oescriptimpl.OesNet(dispatcher, false, nil, nil)
assert(net ~= nil)
json = oescriptimpl.OesServer(dispatcher, "oejson://0.0.0.0:6666", net, store)
assert(json ~= nil)
json:addKernel("myspace", kernel)

--shutdown
print("===> swig api tests shutting down")

