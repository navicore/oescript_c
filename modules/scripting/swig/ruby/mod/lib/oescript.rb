# 
# The contents of this file are subject to the Apache License
# Version 2.0 (the "License"); you may not use this file except in
# compliance with the License. You may obtain a copy of the License 
# from the file named COPYING and from http://www.apache.org/licenses/.
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# The Original Code is OeScript.
#
# The Initial Developer of the Original Code is OnExtent, LLC.
# Portions created by OnExtent, LLC are Copyright (C) 2008-2010
# OnExtent, LLC. All Rights Reserved.
# 

##########################################
##########################################
## note!!!! incomplete broken abandoned ##
## back to the drawing board never      ##
## worked in the first place            ##
##########################################
##########################################

module Oescript
require 'oescriptimpl'

DEFAULT_DUR     = 1000 * 60 * 60 * 24   # 24 hours
DEFAULT_TXN_DUR = 1000 * 60 * 10        # 10 minutes
DEFAULT_TIMEOUT = 1000 * 10             # 10 seconds
UNDEF_TXT       = "__NULL__"
SKIP_TXT        = "_"
WILD_CARD_TXT   = "_"

class OesKernel

    attr_reader :impl

    def initialize(dispatcher, tdispatcher, store, serialize, netspec, net)

        if net == nil
            netimpl = nil
        else
            netimpl = net.impl
        end

        if store == nil
            storeimpl = nil
        else
            storeimpl = store.impl
        end

        if tdispatcher == nil
            tdispatcherimpl = nil
        else
            tdispatcherimpl = tdispatcher.impl
        end

        @impl = Oescriptimpl::OesKernel.new(dispatcher.impl, tdispatcherimpl, storeimpl, netspec, netimpl)

        @return_attrs = !serialize
        @return_bytes = serialize
    end

    def writeData(data, dur=DEFAULT_DUR)
        slist = Oescriptimpl::OesDataObject.new()
        if @return_bytes
            _serialize_write_data(data, slist)
        end
        if data.instance_of?(Hash)
            return _write_dict(data, dur, slist)
        else
            return _write_tuple(data, dur, slist)
        end
    end

    def readData(templ, timeout=DEFAULT_TIMEOUT, nresults=1)
        return _get(templ, timeout, nresults, 0)
    end

    def takeData(templ, timeout=DEFAULT_TIMEOUT, nresults=1)
        return _get(templ, timeout, nresults, 1)
    end

    def begin(dur=DEFAULT_TXN_DUR)
        #txnimpl = @impl.begin(dur)
        return OesTxn(txnimpl)
    end

    private

    def _fix_write_entry(item)
        if item == nil
            return UNDEF_TXT
        elsif item.instance_of?(Hash)
            return SKIP_TXT
        elsif item.instance_of?(Array)
            return SKIP_TXT
        else
            return item.to_s
        end
    end

    def _fix_read_entry(item)
        if item == nil
            return UNDEF_TXT
        elsif item.instance_of?(Hash)
            raise Exception.new("template fields must be strings or numbers")
        elsif item.instance_of?(Array)
            raise Exception.new("template fields must be strings or numbers")
        else
            return item.to_s
        end
    end

    def _write_tuple(data, dur, slist)
        data.each do |item|
            slist.add_attr(_fix_write_entry(item))
        end

        leaseimpl = @impl.write(slist, dur, 1)
        return OesLease.new(leaseimpl)
    end

    def _write_dict(data, dur, slist)
        data.each do |key, value|
            slist.add_attr(_fix_write_entry(key))
            slist.add_attr(_fix_write_entry(value))
        end

        leaseimpl = @impl.write(slist, dur, 0)
        return OesLease.new(leaseimpl)
    end

    def _get(templ, timeout, nresults, destroy)
        print("ejs _get\n")
        slistlist = Oescriptimpl::OesDataObjectList.new()
        if templ.instance_of?(Hash)
            return _get_dict(templ, timeout, nresults, destroy, slistlist)
        else
            return _get_tuple(templ, timeout, nresults, destroy, slistlist)
        end
    end

    def _get_dict(templ, timeout, nresults, destroy, slistlist)
        print("ejs _get_dict\n")
        slist = slistlist.newDataObject() 
        templ.each do |key, value|
            slist.add_attr(_fix_read_entry(key))
            slist.add_attr(_fix_read_entry(value))
        end

        if destroy
            results = self.__impl.take(slistlist, timeout, 0,
                nresults, self.RETURN_ATTRS, self.RETURN_BYTES)
        else
            results = self.__impl.read(slistlist, timeout, 0,
                nresults, self.RETURN_ATTRS, self.RETURN_BYTES)
        end

        if self.RETURN_BYTES
            return DataObject(results, _populate_do_from_bytes)
        end
        return DataObject(results, _populate_map_seq)
    end

    def _get_tuple(templ, timeout, nresults, destroy, slistlist)
        print("ejs _get_tuple\n")
        slist = slistlist.newDataObject() 
        templ.each do |item|
            slist.add_attr(_fix_read_entry(item))
        end

        if destroy
            results = @impl.take(slistlist, timeout, 1,
                nresults, @return_attrs, @return_bytes)
        else
            results = @impl.read(slistlist, timeout, 1,
                nresults, @return_attrs, @return_bytes)
        end

        if @return_bytes
            return DataObject.new(results, _populate_do_from_bytes)
        end
        #todo: ejs
        # make a base DataObject class, and Array and Hash subs
            # make a base DataObject class, and Array and Hash subs
            # make a base DataObject class, and Array and Hash subs
            # make a base DataObject class, and Array and Hash subs
            # make a base DataObject class, and Array and Hash subs
            # make a base DataObject class, and Array and Hash subs
            # make a base DataObject class, and Array and Hash subs
            # make a base DataObject class, and Array and Hash subs
        return DataObject.new( results, Proc.new {_populate_seq_seq} )
    end

    def _populate_seq_seq(dobj_col)
        iter = dobj_col.iter()
        array = []
        while iter.hasMore()
            array.append(_DataObject(iter.nextItem(), _populate_seq))
        end
        return array
    end

end

#python pack data into slist's bytes
def _serialize_write_data(data, slist)
    bytes = pickle.dumps(data)
    nbytes = len(bytes)
    if not nbytes
        raise Exception("write DataObject has no attachment")
    end
    slist.set_nbytes(nbytes)
    slist.set_bytes(bytes)
    return
end

def _printme(dobj)
    print "DEBUG PRINT"
    iter = dobj.iter()
    array = []
    while iter.hasMore()
        thing = iter.nextItem()
        print "  DEBUG %s" % thing
    end
    return 
end

#python unpack
def _populate_do_from_bytes(dobj_col)
    iter = dobj_col.iter()
    array = []
    while iter.hasMore()
        dobj = iter.nextItem()
        nbytes = dobj.get_nbytes()
        if not nbytes
            print "  ****************************************  "
            _printme(dobj) 
            print "  ****************************************  "
            raise Exception("read DataObject has no attachment")
        end
        pyobj = pickle.loads(dobj.get_bytes())
        array.append(pyobj)
    end
    return array
end


def _populate_map_seq(dobj_col)
    iter = dobj_col.iter()
    array = []
    while iter.hasMore()
        array.append(_DataObject(iter.nextItem(), _populate_map))
    end
    return array
end


def _fix_result_entry(item)
    if item == UNDEF_TXT
        return None
    else
        return item
    end
end



def _populate_seq(dobj)
    iter = dobj.iter()
    array = []
    while iter.hasMore()
        array.append( _fix_result_entry( iter.nextItem() ) )
    end
    return array
end


def _populate_map(dobj)
    iter = dobj.iter()
    dict = {} 
    while iter.hasMore()
        key = _fix_result_entry(iter.nextItem())
        val = _fix_result_entry(iter.nextItem())
        dict[key] = val
    end
    return dict
end


class DataObject

    #attr_reader :impl

    def initialize(dobj, &lazy_initializer)
        print("ejs DataObject.new\n")
        @impl = dobj
        @isinit = False
        @lazy_init = Proc.new(lazy_initializer @impl)
    end

    private

    def _populate()
        @collection = @lazy_init.call
        @isinit = True
    end

    def __getitem__(key)
        if !@isinit
            _populate()
        end
        return @collection[key]
    end

    def __len__()
        if !@isinit
            _populate()
        end
        return len(@collection)
    end

    def __iter__()
        if !@isinit
            _populate()
        end
        return @collection.__iter__()
    end
end


class OesTxn

    attr_reader :impl

    def initialize (impl)
        @impl = impl
    end

    def status()
        s = @impl.status()
        if s == oescriptimpl.ACTIVE
            return "A"
        end
        if s == oescriptimpl.COMMITTED
            return "C"
        end
        if s == oescriptimpl.ROLLEDBACK
            return "R"
        end
        raise Exception("invalid txn status")
    end

    def tid()
        return @impl.tid()
    end

    def exptime()
    end

    def commit()
        @impl.commit()
    end

    def rollback()
        @impl.rollback()
    end
end


class OesLease

    attr_reader :impl

    def initialize(impl)
        @impl = impl
    end

    def lid()
        return @impl.lid()
    end

    def exptime()
    end

    def cancel()
        @impl.cancel()
    end
end


class OesNet

    attr_reader :impl

    def initialize(dispatcher, use_ssl, cert, pkey)
        @impl = Oescriptimpl::OesNet.new(dispatcher.impl, use_ssl, cert, pkey)
    end
end


class OesServer

    attr_reader :impl

    def initialize(kernel, dispatcher, netspec, net, store)
        @impl = Oescriptimpl::OesServer.new(
            kernel.impl,
            dispatcher.impl,
            netspec,
            net.impl,
            store.impl)
    end
end


class OesSigHandler

    def initialize(file_name = "mysigfile.pid")
        @impl = Oescriptimpl::OesSigHandler.new(file_name)
    end
end


class OesStore

    attr_reader :impl

    def initialize (persistence_level, dir_name, is_threaded)
        @impl = Oescriptimpl::OesStore.new(persistence_level, dir_name, is_threaded)
    end
end


class OesThreadDispatcher

    attr_reader :impl

    def initialize (nthreads)
        @impl = Oescriptimpl::OesThreadDispatcher.new(nthreads)
    end

    def start()
        @impl.start
    end

    def stop()
        @impl.stop
    end
end


class OesDispatcher

    attr_reader :impl

    def initialize(is_async, name)
        @impl = Oescriptimpl::OesDispatcher.new(is_async, name)
    end

    def start()
        @impl.start()
    end

    def stop()
        @impl.stop()
    end
end

end

