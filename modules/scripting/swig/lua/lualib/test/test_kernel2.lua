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

sz = 16
--threads = 4
threads = 1

d = oescript.OesDispatcher:new(1, "kernel dispatcher")
d:start()
td = oescript.OesThreadDispatcher:new(threads)
td:start()
store = oescript.OesStore:new(1, "data", true)

k = oescript.OesKernel:new(d, td, store, false)

------- test write ---------
td1 = {name="me", addr="somewhere"}
templ = {addr="somewhere"}

leases = {}
for i =1, sz do
  td1["id"] = i
  leases[i] = k:writeTable(td1, 50000)
end

for i =1, sz do
  print("lease:", leases[i]:id(50000))
end

results = {}
for i =1, sz do
  results[i] = k:takeTable(templ, 50000)
end

for i =1, sz do
  r = results[i]()
  print(i, "name:", r["name"], "id:", r["id"])
end

