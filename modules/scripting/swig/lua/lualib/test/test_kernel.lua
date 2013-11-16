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

d = oescript.OesDispatcher:new(1, "kernel dispatcher")
d:start()
--td = oescript.OesThreadDispatcher:new(10)
td = oescript.OesThreadDispatcher:new(1)
td:start()
store = oescript.OesStore:new(1, "data", true)

k = oescript.OesKernel:new(d, td, store, false, nil, nil)


------- test write ---------
td1 = {name="mee", addr="somewhere"}
l1 = k:writeTable(td1, 5000)
l1_lid = l1:id()
--l1_exptime = l1:exptime()
print("l1 lid: ", l1_lid)
print("       now: ", os.date("%c"))
--print("   exptime: ", os.date("%c", l1:exptime()))

array1 = {"one", "two", "three", "four", "five", 666}
l2 = k:writeArray(array1, 9900)
l2_lid = l2:id()
--l2_exptime = l2:exptime()
--print("l2 lid: ", l2_lid, "l2 id(): ", l2:id(), " l2 exp: ", l2_exptime)
--print("l1 lid: ", l1_lid, "l1 id(): ", l1:id(), " l1 exp: ", l1_exptime)
assert(l2_lid == l2:id())
assert(l1_lid == l1:id()) --bug ejs l2 lid steps on l1 lid

------- test read ---------
template1 = {name="mee"}
foundme = k:readTable(template1);
assert(foundme)
for r in foundme do
	for k,v in pairs(r) do
	    print("got k: ", k, "got v: ", v)
        assert(r["name"] == "mee")
        assert(r["name"] ~= "somewhere")
        assert(r["addr"] == "somewhere")
        assert(r["addr"] == td1["addr"])
	end
end

print("---")

notfoundme = k:readTable({miss="ed"},502);
assert(not notfoundme())

------- test lease cancel  ---------
canfindme = k:readTable(template1);
assert(canfindme() ~= nil)
--print("lease expTime (before cancel): ", os.date("%c", l1:exptime()))
l1:cancel() 
--os.execute("sleep 2")
--print("lease expTime (after cancel, before read): ", os.date("%c", l1:exptime()))
print("lease expTime (after cancel, before read): ", os.date("%c", l1:id()))
cannotfindme = k:readTable(template1,1)

shouldbenil = cannotfindme()
assert(shouldbenil == nil)

lease2 = k:writeTable(td1, 5000) --put a new one back for tests below
lease2:id()

------- test take ---------
tookme = k:takeTable(template1,5003);
assert(tookme()["name"] == "mee")
gone = k:readTable(template1,50);
assert(gone() == nil)

------- test write rollback ---------
txn1 = k:begin()
assert(txn1 ~= nil)
w1 = {name="walter", addr="pelham", tel=789}
k:writeTable(w1, 60000)
wt = {name="walter"}
w2 = k:readTable(wt, 100)
assert(w2() == nil)
txn1:rollback()
--k:commit(tid)
w2 = k:readTable(wt, 100)
assert(w2)
assert(w2() == nil)

------- test write commit ---------
txn2 = k:begin()
assert(txn2 ~= nil)
print("tid:", txn2:id())

r1 = {name="omar", addr="austin", tel=123 }
rtmp = {addr="austin"}

k:writeTable(r1, 60000)
t2 = k:readTable(rtmp, 100)
assert(t2() == nil)

txn2:commit()
print("committed txn", txn2:status())
t3 = k:readTable(rtmp, 5005)()
assert(t3 ~= nil)
name = t3["name"]
assert(name == "omar")
print("name:", name)

------- test take rollback ---------
r1 = {name="kayham", tel=456, addr="dallas"}
l = k:writeTable(r1, 60000)
lv = l:id(5006)
txn3 = k:begin()
rtmp = {tel=456}
t2 = k:takeTable(rtmp, 5000)()
assert(t2 ~= nil)
name = t2["name"]
assert(name == "kayham")
print("name:", name)

t3 = k:readTable(rtmp, 50)()
assert(t3 == nil)
t3 = k:takeTable(rtmp, 0)()
assert(t3 == nil)

txn3:rollback()

t4 = k:readTable(rtmp, 50000)()
assert(t4 ~= nil)
assert(t4["name"] == "kayham")

t6 = k:readTable(rtmp, 50000)()
print("t6:", t6)
assert(t6 ~= nil)
assert(t6["name"] == "kayham")
print(t6["name"] == "kayham")
print(t6["name"])

