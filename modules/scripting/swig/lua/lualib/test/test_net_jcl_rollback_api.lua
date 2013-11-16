#!../oescript
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

function printDobjTblList(dlist)
    assert(dlist)
	print("...dolist...")
    for r in dlist do
	    for k,v in pairs(r) do
	        print("  got k: ", k, "got v: ", v)
	    end
	end
end

username = "Doghouse Reilly"
pwd = "john bonham rocks"

loginmod = oescript.OesBuiltInLoginModule:new()
loginmod:addAccount("users", username, pwd)
loginmod:addPrivilege("somespace", "users", '{"t":"cmd", "n":"connect"}')
loginmod:addPrivilege("somespace", "users", '{"t":"cmd", "n":"disconnect"}')
loginmod:addPrivilege("somespace", "users", '{"t":"cmd", "n":"put"}')
loginmod:addPrivilege("somespace", "users", '{"t":"cmd", "n":"get"}')
loginmod:addPrivilege("somespace", "users", '{"t":"cmd", "n":"start"}')
loginmod:addPrivilege("somespace", "users", '{"t":"cmd", "n":"utxn"}')
loginmod:addPrivilege("somespace", "users", '{"t":"cmd", "n":"ulease"}')

d = oescript.OesDispatcher:new(1, "kernel dispatcher")
d:start()
td = oescript.OesThreadDispatcher:new(10)
td:start()
store = oescript.OesStore:new(1, "data", true)

k = oescript.OesKernel:new(d, td, store, false, nil, nil)
------- sock server ---------
net = oescript.OesNet:new(d, false, nil, nil)
jcl = oescript.OesServer:new(d, "oejcl://0.0.0.0:6669/somespace", net, store)
assert(jcl)
jcl:addLoginModule(loginmod)

jcl:addKernel("somespace", k)

------- sock client ---------
clnet = oescript.OesNet:new(d, false, nil, nil)
assert(clnet)
myclient = oescript.OesKernel:new(d, nil, nil, false,
                           "oejcl://127.0.0.1:6669/somespace", clnet, username, pwd)
assert(myclient)

------- test take rollback ---------
r1 = {name="kayham", tel=456, addr="dallas"}
l = myclient:writeTable(r1, 60000)
lv = l:id(5006)
--lv = l:id()
txn3 = myclient:begin()
--rtmp = {tel=456}
rtmp = {addr="dallas", tel=456}
t2 = myclient:takeTable(rtmp, 5000)()
assert(t2 ~= nil)
name = t2["name"]
assert(name == "kayham")
print("name:", name)

t3 = myclient:readTable(rtmp, 50)()
assert(t3 == nil)
t3 = myclient:takeTable(rtmp, 0)()
assert(t3 == nil)

txn3:rollback()

t4 = myclient:readTable(rtmp, 50000)()
assert(t4 ~= nil)
assert(t4["name"] == "kayham")

t6 = myclient:readTable(rtmp, 50000)()
assert(t6 ~= nil)
assert(t6["name"] == "kayham")

t7 = myclient:readTable(rtmp, 50000)()
assert(t7 ~= nil)
assert(t7["name"] == "kayham")

t8 = myclient:readTable(rtmp, 50000)()
assert(t8 ~= nil)
assert(t8["name"] == "kayham")

print(t6["name"] == "kayham")
print(t6["name"] == "kayham")
print(t6["name"] == "kayham")
print(t6["name"] == "kayham")
print(t6["name"])
--[[
]]--

