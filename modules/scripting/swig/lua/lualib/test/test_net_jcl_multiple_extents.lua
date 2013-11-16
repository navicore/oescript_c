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

function makeClient(uri, username, pwd)
    local d = oescript.OesDispatcher:new(1, "client dispatcher")
    d:start()
    local net = oescript.OesNet:new(d, false, nil, nil)
    assert(net)
    local client = oescript.OesKernel:new(d, nil, nil, false, uri, net, username, pwd)
    assert(client)
    return client
end

function printDobjTblList(dlist)
    assert(dlist)
	print("...dolist...")
    for r in dlist do
	    for k,v in pairs(r) do
	        print("  got k: ", k, "got v: ", v)
	    end
	end
end

username1 = "me" -- can do stuff in both extents
username2 = "you" -- can only do stuff in 1 extent
pwd = "john bonham rocks"

loginmod = oescript.OesBuiltInLoginModule:new()

loginmod:addAccount("users1", username1, pwd)
loginmod:addAccount("users2", username2, pwd)

loginmod:addPrivilege("somespace", "users1", '{"t":"cmd", "n":"connect"}')
loginmod:addPrivilege("somespace", "users1", '{"t":"cmd", "n":"disconnect"}')
loginmod:addPrivilege("somespace", "users1", '{"t":"cmd", "n":"put"}')
loginmod:addPrivilege("somespace", "users1", '{"t":"cmd", "n":"get"}')
loginmod:addPrivilege("somespace", "users1", '{"t":"cmd", "n":"start"}')
loginmod:addPrivilege("somespace", "users1", '{"t":"cmd", "n":"utxn"}')
loginmod:addPrivilege("somespace", "users1", '{"t":"cmd", "n":"ulease"}')
loginmod:addPrivilege("someotherspace", "users1", '{"t":"cmd", "n":"connect"}')
loginmod:addPrivilege("someotherspace", "users1", '{"t":"cmd", "n":"disconnect"}')
loginmod:addPrivilege("someotherspace", "users1", '{"t":"cmd", "n":"put"}')
loginmod:addPrivilege("someotherspace", "users1", '{"t":"cmd", "n":"get"}')
loginmod:addPrivilege("someotherspace", "users1", '{"t":"cmd", "n":"start"}')
loginmod:addPrivilege("someotherspace", "users1", '{"t":"cmd", "n":"utxn"}')
loginmod:addPrivilege("someotherspace", "users1", '{"t":"cmd", "n":"ulease"}')

loginmod:addPrivilege("someotherspace", "users2", '{"t":"cmd", "n":"connect"}')
loginmod:addPrivilege("someotherspace", "users2", '{"t":"cmd", "n":"disconnect"}')
--loginmod:addPrivilege("someotherspace", "users2", '{pe":"cmd", me":"put"}')
loginmod:addPrivilege("someotherspace", "users2", '{"t":"cmd", "n":"get"}')
loginmod:addPrivilege("someotherspace", "users2", '{"t":"cmd", "n":"start"}')
loginmod:addPrivilege("someotherspace", "users2", '{"t":"cmd", "n":"utxn"}')
loginmod:addPrivilege("someotherspace", "users2", '{"t":"cmd", "n":"ulease"}')

d = oescript.OesDispatcher:new(1, "kernel dispatcher")
d:start()
td = oescript.OesThreadDispatcher:new(10)
td:start()
store1 = oescript.OesStore:new(1, "somespace", true)
store2 = oescript.OesStore:new(1, "someotherspace", true)
sysstore = oescript.OesStore:new(3, "data", true)

k1 = oescript.OesKernel:new(d, td, store1, false, nil, nil)
k2 = oescript.OesKernel:new(d, td, store2, false, nil, nil)

------- sock server ---------
net = oescript.OesNet:new(d, false, nil, nil)
jcl = oescript.OesServer:new(d, "oejcl://0.0.0.0:6669", net, sysstore)
assert(jcl)
jcl:addLoginModule(loginmod)
jcl:addKernel("somespace", k1)
jcl:addKernel("someotherspace", k2)

------- sock client ---------
--should connect and have r/w privs
myclient1 = makeClient( "oejcl://127.0.0.1:6669/somespace", username1, pwd)
assert(myclient1)
--should connect and have r/w privs
myclient2 = makeClient( "oejcl://127.0.0.1:6669/someotherspace", username1, pwd)
assert(myclient2)
--should connect and have readonly privs
yourclient2 = makeClient( "oejcl://127.0.0.1:6669/someotherspace", username2, pwd)
assert(yourclient2)
--should connect and have no privs
yourclient1 = makeClient( "oejcl://127.0.0.1:6669/somespace", username2, pwd)
assert(yourclient1)
--should not connect
--bogusclient = makeClient( "oejcl://127.0.0.1:6669/somespace", "fool", pwd)
--assert(bogusclient)

-- user 1 can read and write in both spaces
r1 = {name="akayham", tel=456, addr="dallased"}
l = myclient1:writeTable(r1, 60000)
lv = l:id(5006)
rtmp1 = {addr="dallased", tel=456}
t2 = myclient1:takeTable(rtmp1, 5000)()
assert(t2 ~= nil)
name = t2["name"]
assert(name == "akayham")
print("name:", name)

r2 = {name="bkayham", tel=456, addr="dallasor"}
l = myclient2:writeTable(r2, 60000)
lv = l:id(5006)
rtmp2 = {addr="dallasor", tel=456}
t2 = myclient2:readTable(rtmp2, 5000)()
assert(t2 ~= nil)
name = t2["name"]
assert(name == "bkayham")
print("name:", name)
--should not be able to read the table written to one space from the other
t3 = myclient2:readTable(rtmp1, 0)()
assert(t3 == nil)

-- user 2 can only read in 1 space and do nothing in the other
r3 = {name="ckayham", tel=456, addr="dallased"}
--l = yourclient1:writeTable(r3, 60000)
--lv = l:id(5006)
--l = yourclient2:writeTable(r3, 60000)
--lv = l:id(5006)
t4 = yourclient2:readTable(rtmp2, 5000)()
assert(t4 ~= nil)
name = t4["name"]
assert(name == "bkayham")
print("name:", name)

--[[
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
]]--

