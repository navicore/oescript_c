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
module("oescript", package.seeall)
require "oescriptimpl"

_DEFAULT_DUR     = 1000 * 60 * 60 * 24   -- 24 hours
_DEFAULT_TXN_DUR = 1000 * 60 * 10        -- 10 minutes
_DEFAULT_TIMEOUT = 1000 * 10             -- 10 seconds
_UNDEF_TXT       = "__NULL__"
_SKIP_TXT        = "_"
_WILD_CARD_TXT   = "_"

OesBuiltInLoginModule = {}

function oescript.OesBuiltInLoginModule:new()
    o = {}
    setmetatable(o, self)
    self.__index = self
    o._impl = oescriptimpl.OesBuiltInLoginModule()
    o.loginmod = o._impl:getModule()
    return o
end

function oescript.OesBuiltInLoginModule:addAccount(group, username, pwd)
    self._impl:addAccount(group, username, pwd)
    return o
end

function oescript.OesBuiltInLoginModule:addPrivilege(extent, group, mtemplate)
    self._impl:addPrivilege(extent, group, mtemplate)
    return o
end


OesSigHandler = {}

function oescript.OesSigHandler:new (filename, appname, lvl)
    o = {}
    setmetatable(o, self)
    self.__index = self
    o._impl = oescriptimpl.OesSigHandler(filename, appname, lvl)
    return o
end

function oescript.OesSigHandler:add(obj)
    self._impl:add(obj._impl)
end


OesDispatcher = {}

function oescript.OesDispatcher:new (nthreads, name)
    o = {}
    setmetatable(o, self)
    self.__index = self
    o._impl = oescriptimpl.OesDispatcher(nthreads, name)
    return o
end

function oescript.OesDispatcher:start ()
    self._impl:start()
    return o
end


OesThreadDispatcher = {}

function oescript.OesThreadDispatcher:new (nthreads)
    o = {}
    setmetatable(o, self)
    self.__index = self
    o._impl = oescriptimpl.OesThreadDispatcher(nthreads)
    return o
end

function oescript.OesThreadDispatcher:start ()
    self._impl:start()
    return o
end


OesStore = {}

function oescript.OesStore:new (feature_level, dir, threaded)
    o = {}
    setmetatable(o, self)
    self.__index = self
    o._impl = oescriptimpl.OesStore(feature_level, dir, threaded)
    return o
end

OesJsonKernel = {}

function oescript.OesJsonKernel:new (kernel)
    o = {}
    setmetatable(o, self)
    self.__index = self
    o._impl = oescriptimpl.OesJsonKernel(kernel._impl)
    return o
end

function oescript.OesJsonKernel:exec(json)
    local result = self._impl:exec(json)
    return oescript.OesFuture:new(result)
end


OesNet = {}

function oescript.OesNet:new (dispatcher, use_ssl, cert, pkey)
    o = {}
    setmetatable(o, self)
    self.__index = self
    o._impl = oescriptimpl.OesNet(dispatcher._impl, use_ssl, cert, pkey)
    return o
end

function oescript.OesNet:setName(name)
    self._impl:setName(name)
    return o
end


OesServer = {}

function oescript.OesServer:new (dispatcher, spec, net, store)
    o = {}
    setmetatable(o, self)
    self.__index = self
    o._impl = oescriptimpl.OesServer(dispatcher._impl, spec, net._impl, store._impl)
    return o
end

function oescript.OesServer:addLoginModule(m)
    self._impl:addLoginModule(m.loginmod)
    return o
end

function oescript.OesServer:addKernel(kernelextname, extentkernel)
    self._impl:addKernel(kernelextname, extentkernel._impl)
    return o
end


OesFuture = {}

function oescript.OesFuture:new(impl)
    o = {}
    setmetatable(o, self)
    self.__index = self
    o._impl = impl
    return o
end

function oescript.OesFuture:getString(timeout)
    local json = self._impl:getString(timeout)
    return json
end

OesLease = {}

function oescript.OesLease:new(impl)
    o = {}
    setmetatable(o, self)
    self.__index = self
    o._impl = impl
    return o
end

function oescript.OesLease:id()
    local lid = self._impl:lid()
    return lid
end

-- returns secs since epoch
-- function oescript.OesLease:exptime()
--     local exptime =  self._impl:exptime()
--     return exptime / 1000
-- end

function oescript.OesLease:cancel()
    return self._impl:cancel()
end


OesTxn = {}

function oescript.OesTxn:new(impl)
    o = {}
    setmetatable(o, self)
    self.__index = self
    o._impl = impl
    return o
end

function oescript.OesTxn:status()
    s = self._impl:status()
    if s == oescriptimpl.ACTIVE then
        return "A"
    end
    if s == oescriptimpl.COMMITTED then
        return "C"
    end
    if s == oescriptimpl.ROLLEDBACK then
        return "R"
    end
    assert(false)
end

function oescript.OesTxn:id()
    local tid = self._impl:tid()
    return tid
end

-- returns secs since epoch
-- function oescript.OesTxn:exptime()
--     local exptime =  self._impl:exptime()
--     return exptime / 1000
-- end

function oescript.OesTxn:commit()
    return self._impl:commit()
end

function oescript.OesTxn:rollback()
    return self._impl:rollback()
end


OesKernel = {}

function oescript.OesKernel:new (dispatcher, tdispatcher, store, serialize,
                                 netspec, net, username, pwd)
    if store == nil then
        storeimpl = nil
    else
        storeimpl = store._impl
    end
    if tdispatcher == nil then
        tdimpl = nil
    else
        tdimpl = tdispatcher._impl
    end
    if net == nil then
        netimpl = nil
    else
        netimpl = net._impl
    end
    o = {}
    setmetatable(o, self)
    self.__index = self
    o._impl = oescriptimpl.OesKernel(dispatcher._impl, tdimpl, storeimpl,
                                     netspec, netimpl, username, pwd)
    o._RETURN_ATTRS = not serialize
    o._RETURN_BYTES = serialize
    return o
end


function _fix_result_entry(item)
    --todo
    return item
end

function _fix_read_entry(item)
    --todo
    return item
end

function _fix_write_entry(item)
    if item == nil then
        return _UNDEF_TXT
    elseif type(item) == "table" then  -- is there a better way??! ejs
        return _SKIP_TXT
    else
        return item
    end
end


function _ser_bytes(tbl, dobj)
    assert(false)
end

function oescript.OesKernel:writeArray(tbl, dur)
    return self:_put(tbl, dur, true)
end

function oescript.OesKernel:writeTable(tbl, dur)
    return self:_put(tbl, dur, false)
end

function oescript.OesKernel:_put(tbl, dur, istuple)

    local dobj = oescriptimpl.OesDataObject()

    if self._RETURN_BYTES then
        _ser_bytes(tbl, dobj)
    end

    if istuple then
        for _,v in ipairs(tbl) do
            dobj:add_attr(_fix_write_entry(v))
        end
    else
        for k,v in pairs(tbl) do
            dobj:add_attr(_fix_write_entry(k))
            dobj:add_attr(_fix_write_entry(v))
        end
    end

    local leaseimpl = self._impl:write(dobj, dur, istuple) 
    return oescript.OesLease:new(leaseimpl)
end

function _deser_dobj_array(dobj)
    local result = {}
    local diter = dobj:iter()
    local i = 0
    while diter:hasMore() do
        v = diter:nextItem()
	    result[i] = v
        i = i + 1
    end
    return result
end

function _deser_dobj_tbl(dobj)
    local result = {}
    local diter = dobj:iter()
    while diter:hasMore() do
        k = diter:nextItem()
        v = diter:nextItem()
	    result[k] = v
    end
    return result
end

function _create_array_template(tbl, dataobject)
    for _,v in ipairs(tbl) do
        dataobject:add_attr(_fix_read_entry(v))
    end
end

function _create_tbl_template(tbl, dataobject)
    for k,v in pairs(tbl) do
        dataobject:add_attr(_fix_read_entry(k))
        dataobject:add_attr(_fix_read_entry(v))
    end
end

function oescript.OesKernel:readArray(tbl, timeout)
    return self:_get(tbl, timeout, false, true)
end

function oescript.OesKernel:readTable(tbl, timeout)
    return self:_get(tbl, timeout, false, false)
end

function oescript.OesKernel:takeArray(tbl, timeout)
    return self:_get(tbl, timeout, true, true)
end

function oescript.OesKernel:takeTable(tbl, timeout)
    return self:_get(tbl, timeout, true, false)
end

function oescript.OesKernel:_get(tbl, timeout, destroy, istuple)

    local timeout = timeout or 10000
    local nresults = nresults or 1

    local dataobjectlist = oescriptimpl.OesDataObjectList();
    local dataobject = dataobjectlist:newDataObject() 
    if istuple then
        _create_array_template(tbl, dataobject)
    else
        _create_tbl_template(tbl, dataobject)
    end

    local results
    if destroy then
        results = self._impl:take(dataobjectlist, timeout, istuple,
            nresults, self._RETURN_ATTRS, self._RETURN_BYTES)
    else
        results = self._impl:read(dataobjectlist, timeout, istuple,
            nresults, self._RETURN_ATTRS, self._RETURN_BYTES)
    end

    local iter = nil
    return function ()
        iter = iter or results:iter()
        if not iter:hasMore() then
            return nil
        end
        if self._RETURN_BYTES then
            return _deser_dobj_bytes(iter:nextItem())
        else
            if istuple then
                return _deser_dobj_array(iter:nextItem())
            else
                return _deser_dobj_tbl(iter:nextItem())
            end
        end
    end
end

function oescript.OesKernel:begin(timeout)
    local timeout = timeout or 10000
    local timpl = self._impl:begin(timeout)
    return oescript.OesTxn:new(timpl)
end

function getopt( arg, options )
  local tab = {}
  for k, v in ipairs(arg) do
    if string.sub( v, 1, 2) == "--" then
      local x = string.find( v, "=", 1, true )
      if x then tab[ string.sub( v, 3, x-1 ) ] = string.sub( v, x+1 )
      else      tab[ string.sub( v, 3 ) ] = true
      end
    elseif string.sub( v, 1, 1 ) == "-" then
      local y = 2
      local l = string.len(v)
      local jopt
      while ( y <= l ) do
        jopt = string.sub( v, y, y )
        if string.find( options, jopt, 1, true ) then
          if y < l then
            tab[ jopt ] = string.sub( v, y+1 )
            y = l
          else
            tab[ jopt ] = arg[ k + 1 ]
          end
        else
          tab[ jopt ] = true
        end
        y = y + 1
      end
    end
  end
  return tab
end


function basicSerialize( o )

    if type( o ) == "number" then
        return tostring( o )
    else
        return string.format( "%q", o )
    end
end

function save(value, saved, name, buf)

    saved = saved or {}
    name = name or "__oe_rt"
    buf = buf or {}

    buf[#buf + 1] = name
    buf[#buf + 1] = " = "
    if type( value ) == "number" or type( value ) == "string" then
        buf[#buf + 1] = basicSerialize( value )
        buf[#buf + 1] = "\n"
    elseif type( value ) == "table" then
        if saved[value] then
            buf[#buf + 1] = saved( value )
            buf[#buf + 1] = "\n"
        else
            saved[value] = name
            buf[#buf + 1] = "{}\n"
            for k,v in pairs(value) do
                k = basicSerialize( k )
                local fname = string.format( "%s[%s]", name, k )
                save( v, saved, fname, buf )
            end
        end
    else
        error( "cannot save a " .. type(value))
    end
    return table.concat(buf)
end

function load(buf)
    if buf == nil then
        return nil
    else
        f = loadstring(buf)
        if f == nil then
            return nil
        else
            f()
            return __oe_rt
        end
    end
end

