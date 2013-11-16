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

username1 = "seeme" -- can do stuff in both extents
username2 = "seeyou" -- can only do stuff in 1 extent
pwd = "john bonham rocks?"

loginmod = oescript.OesBuiltInLoginModule:new()

loginmod:addAccount("users1", username1, pwd)
loginmod:addAccount("users2", username2, pwd)

loginmod:addPrivilege("someplace", "users1", '{"t":"cmd", "n":"connect"}')
loginmod:addPrivilege("someplace", "users1", '{"t":"cmd", "n":"disconnect"}')
loginmod:addPrivilege("someplace", "users1", '{"t":"cmd", "n":"put"}')
loginmod:addPrivilege("someplace", "users1", '{"t":"cmd", "n":"get"}')
loginmod:addPrivilege("someplace", "users1", '{"t":"cmd", "n":"start"}')
loginmod:addPrivilege("someplace", "users1", '{"t":"cmd", "n":"utxn"}')
loginmod:addPrivilege("someplace", "users1", '{"t":"cmd", "n":"ulease"}')

loginmod:addPrivilege("someplace", "users2", '{"t":"cmd", "n":"connect"}')
loginmod:addPrivilege("someplace", "users2", '{"t":"cmd", "n":"disconnect"}')
--loginmod:addPrivilege("someplace", "users2", '{pe":"cmd", me":"put"}')
loginmod:addPrivilege("someplace", "users2", '{"t":"cmd", "n":"get"}')
loginmod:addPrivilege("someplace", "users2", '{"t":"cmd", "n":"start"}')
loginmod:addPrivilege("someplace", "users2", '{"t":"cmd", "n":"utxn"}')
loginmod:addPrivilege("someplace", "users2", '{"t":"cmd", "n":"ulease"}')

d = oescript.OesDispatcher:new(1, "kernel dispatcher")
d:start()
td = oescript.OesThreadDispatcher:new(10)
td:start()
store1 = oescript.OesStore:new(1, "data2", true)
sysstore = oescript.OesStore:new(3, "data", true)

k1 = oescript.OesKernel:new(d, td, store1, false, nil, nil)

------- sock server ---------
net = oescript.OesNet:new(d, false, nil, nil)
jcl = oescript.OesServer:new(d, "oejcl://0.0.0.0:6669", net, sysstore)
assert(jcl)
jcl:addLoginModule(loginmod)
jcl:addKernel("someplace", k1)

------- sock client ---------
--should connect and have r/w privs
myclient1 = makeClient( "oejcl://127.0.0.1:6669/someplace", username1, pwd)
assert(myclient1)
yourclient1 = makeClient( "oejcl://127.0.0.1:6669/someplace", username2, pwd)
assert(yourclient1)

-- user 1 can read and write in both spaces
r1 = {name="dkayham", tel=456, addr="dallasedee"}
l = myclient1:writeTable(r1, 60000)
lv = l:id(5006)
rtmp1 = {addr="dallasedee", tel=456}
t2 = myclient1:readTable(rtmp1, 5000)()
assert(t2 ~= nil)
name = t2["name"]
assert(name == "dkayham")
print("name:", name)

-- user 2 can only read in 1 space and do nothing in the other
t4 = yourclient1:readTable(rtmp1, 5000)()
assert(t4 ~= nil)
name = t4["name"]
assert(name == "dkayham")
print("name:", name)

