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

d = oescript.OesDispatcher:new(1, "kernel dispatcher")
d:start()
td = oescript.OesThreadDispatcher:new(10)
td:start()
store = oescript.OesStore:new(1, "data", true)

k = oescript.OesKernel:new(d, td, store, false, nil, nil)
jk = oescript.OesJsonKernel:new(k);


json_put_cmd = "{                           \
	\"t\":	\"cmd\",                        \
	\"n\":	\"put\",                        \
	\"cid\":	20,                         \
	\"dur\":	50000,                      \
	\"values\":	[\"xtel\", \"8881\"]          \
}"
--\"values\":	[\"tel\", \"456\", \"name\", \"ishtar\"]          \

json_get_cmd = "{                           \
	\"t\":	\"cmd\",                        \
	\"n\":	\"get\",                        \
	\"cid\":	28,                         \
	\"timeout\":	50000,                  \
	\"c\":	1,                              \
	\"rattrs\":	true,                       \
	\"templates\":	[[\"_\", \"8881\"]]    \
}"

f = jk:exec(json_put_cmd);
assert(f)

result = f:getString(10000)
assert(result)
--print("put result: ", result)

f = jk:exec(json_get_cmd);
assert(f)

result = f:getString(1000)
assert(result)
--print("get result: ", result)

