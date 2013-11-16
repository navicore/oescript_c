#!/usr/bin/lua
--[[
 The contents of this file are subject to the Apache License
 Version 2.0 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License 
 from the file named COPYING and from http://www.apache.org/licenses/.
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 The Original Code is OeScript.
 
 The Initial Developer of the Original Code is OnExtent, LLC.
 Portions created by OnExtent, LLC are Copyright (C) 2008-2010
 OnExtent, LLC. All Rights Reserved.
]]--

require "oescript"

signals = oescript.OesSigHandler:new("oeserver.pid", "oeserver", "debug")

-- configure security
spacename = "myspace"
group = "users"
username = "testuser1"
pwd = "john bonham rocks"

loginmod = oescript.OesBuiltInLoginModule:new()
loginmod:addAccount(group, username, pwd)
loginmod:addPrivilege(spacename, group, '{"type":"command", "name":"connect"}')
loginmod:addPrivilege(spacename, group, '{"type":"command", "name":"dis"}')
loginmod:addPrivilege(spacename, group, '{"type":"command", "name":"put"}')
loginmod:addPrivilege(spacename, group, '{"type":"command", "name":"get"}')
loginmod:addPrivilege(spacename, group, '{"type":"command", "name":"sta"}')
loginmod:addPrivilege(spacename, group, '{"type":"command", "name":"upt"}')
loginmod:addPrivilege(spacename, group, '{"type":"command", "name":"upl"}')

-- create core components

d = oescript.OesDispatcher:new(0, "kernel dispatcher")
td = oescript.OesThreadDispatcher:new(10)
td:start()
store = oescript.OesStore:new(3, "/var/oescript/data", true)

k = oescript.OesKernel:new(d, td, store, false, nil, nil)

------- sock server ---------
net = oescript.OesNet:new(d, false, nil, nil)
net:setName("tcp server")
json = oescript.OesServer:new(d, "oejson://0.0.0.0:5555/" .. spacename, net, store)
assert(json)
json:addLoginModule(loginmod)
json:addKernel(spacename, k)

------- ssl sock server ---------
net2 = oescript.OesNet:new(d, true, "cert", "pkey")
net2:setName("tcp ssl server")
json2 = oescript.OesServer:new(d, "oejson://0.0.0.0:5551/" .. spacename, net2, store)
assert(json2)
json2:addLoginModule(loginmod)
json2:addKernel(spacename, k)

d:start()

