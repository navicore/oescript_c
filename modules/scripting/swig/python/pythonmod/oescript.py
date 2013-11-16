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

import oescriptimpl
import collections
import datetime
import pickle as pickle

DEFAULT_DUR     = 1000 * 60 * 60 * 24   # 24 hours
DEFAULT_TXN_DUR = 1000 * 60 * 10        # 10 minutes
DEFAULT_TIMEOUT = 1000 * 10             # 10 seconds
UNDEF_TXT       = "__NULL__"
SKIP_TXT        = "_"
WILD_CARD_TXT   = "_"


class OesKernel(object):

    def __init__ (self, dispatcher, tdispatcher, store, serialize, netspec, net, username, pwd):
        if net == None:
            netimpl = None
        else:
            netimpl = net._OesNet__impl

        if tdispatcher == None:
            tdispatcherimpl = None
        else:
            tdispatcherimpl = tdispatcher._OesThreadDispatcher__impl

        if store == None:
            storeimpl = None
        else:
            storeimpl = store._OesStore__impl

        self.__impl = oescriptimpl.OesKernel(dispatcher._OesDispatcher__impl, 
            tdispatcherimpl , storeimpl, netspec, netimpl, username, pwd)

        self.RETURN_ATTRS = not serialize
        self.RETURN_BYTES = serialize

    def writeData(self, data, dur=DEFAULT_DUR):
        slist = oescriptimpl.OesDataObject();
        if self.RETURN_BYTES:
            _serialize_write_data(data, slist)
        if isinstance(data, tuple):
            return self._write_tuple(data, dur, slist)
        else:
            return self._write_dict(data, dur, slist)

    def readData(self, templ, timeout=DEFAULT_TIMEOUT, nresults=1):
        return self._get(templ, timeout, nresults, 0)

    def takeData(self, templ, timeout=DEFAULT_TIMEOUT, nresults=1):
        return self._get(templ, timeout, nresults, 1)

    def begin(self, dur=DEFAULT_TXN_DUR):
        txnimpl = self.__impl.begin(dur)
        return OesTxn(txnimpl)

    def _write_tuple(self, data, dur, slist):
        for item in data:
            slist.add_attr(_fix_write_entry(item))

        leaseimpl = self.__impl.write(slist, dur, 1)
        return OesLease(leaseimpl)

    def _write_dict(self, data, dur, slist):
        for key in data:
            slist.add_attr(_fix_write_entry(key))
            slist.add_attr(_fix_write_entry(data[key]))

        leaseimpl = self.__impl.write(slist, dur, 0)
        return OesLease(leaseimpl)

    def _get(self, templ, timeout, nresults, destroy):
        slistlist = oescriptimpl.OesDataObjectList();
        if isinstance(templ, tuple):
            return self._get_tuple(templ, timeout, nresults, destroy, slistlist)
        else:
            return self._get_dict(templ, timeout, nresults, destroy, slistlist)

    def _get_dict(self, templ, timeout, nresults, destroy, slistlist):
        slist = slistlist.newDataObject() 
        for key in templ:
            slist.add_attr(_fix_read_entry(key))
            slist.add_attr(_fix_read_entry(templ[key]))

        if destroy:
            results = self.__impl.take(slistlist, timeout, 0,
                nresults, self.RETURN_ATTRS, self.RETURN_BYTES)
        else:
            results = self.__impl.read(slistlist, timeout, 0,
                nresults, self.RETURN_ATTRS, self.RETURN_BYTES)

        if self.RETURN_BYTES:
            return _DataObject(results, _populate_do_from_bytes)
        return _DataObject(results, _populate_map_seq)

    def _get_tuple(self, templ, timeout, nresults, destroy, slistlist):
        slist = slistlist.newDataObject() 
        for item in templ:
            slist.add_attr(_fix_read_entry(item))

        if destroy:
            results = self.__impl.take(slistlist, timeout, 1,
                nresults, self.RETURN_ATTRS, self.RETURN_BYTES)
        else:
            results = self.__impl.read(slistlist, timeout, 1,
                nresults, self.RETURN_ATTRS, self.RETURN_BYTES)

        if self.RETURN_BYTES:
            return _DataObject(results, _populate_do_from_bytes)
        return _DataObject(results, _populate_seq_seq)

#python pack data into slist's bytes
def _serialize_write_data(data, slist):
    bytes = pickle.dumps(data)                        #python2
    #bytes = pickle.dumps(data, 0).decode("utf-8")    #python3
    nbytes = len(bytes)
    if not nbytes:
        raise Exception("write DataObject has no attachment")
    slist.set_nbytes(nbytes)
    slist.set_bytes(bytes)
    return

def _printme(dobj):
    print("DEBUG PRINT")
    iter = dobj.iter()
    array = []
    while iter.hasMore():
        thing = iter.nextItem()
        print("  DEBUG: %s" % thing)
    return 

#python unpack
def _populate_do_from_bytes(dobj_col):
    iter = dobj_col.iter()
    array = []
    while iter.hasMore():
        dobj = iter.nextItem()
        nbytes = dobj.get_nbytes()
        if not nbytes:
            print("  ****************************************  ")
            _printme(dobj) 
            print("  ****************************************  ")
            raise Exception("read DataObject has no attachment")
        #pyobj = pickle.loads(dobj.get_bytes())   #python2
        pyobj = pickle.loads(dobj.get_bytes().encode('utf-8')) #python3
        array.append(pyobj)
    return array


def _populate_map_seq(dobj_col):
    iter = dobj_col.iter()
    array = []
    while iter.hasMore():
        array.append(_DataObject(iter.nextItem(), _populate_map))
    return array


def _populate_seq_seq(dobj_col):
    iter = dobj_col.iter()
    array = []
    while iter.hasMore():
        array.append(_DataObject(iter.nextItem(), _populate_seq))
    return array


def _populate_seq(dobj):
    iter = dobj.iter()
    array = []
    while iter.hasMore():
        array.append( _fix_result_entry( iter.nextItem() ) )
    return array


def _populate_map(dobj):
    iter = dobj.iter()
    dict = {} 
    while iter.hasMore():
        key = _fix_result_entry(iter.nextItem())
        val = _fix_result_entry(iter.nextItem())
        dict[key] = val
    return dict


class _DataObject(collections.Mapping, collections.Sequence):

    def __init__(self, dobj, lazy_initializer):
        self.__impl = dobj
        self._isinit = False
        self._lazy_init = lazy_initializer

    def _populate(self):
        self._collection = self._lazy_init(self.__impl)
        self._isinit = True

    def __getitem__(self, key):
        if not self._isinit:
            self._populate()
        return self._collection[key]

    def __len__(self):
        if not self._isinit:
            self._populate()
        return len(self._collection)

    def __iter__(self):
        if not self._isinit:
            self._populate()
        return self._collection.__iter__()


def _fix_write_entry(item):
    if item == None:
        return UNDEF_TXT
    elif isinstance(item, int):
        return str(item)
    elif not isinstance(item, str):
        return SKIP_TXT
    else:
        return item


def _fix_read_entry(item):
    if item == None:
        return UNDEF_TXT
    elif isinstance(item, int):
        return str(item)
    elif not isinstance(item, str):
        raise TypeError("template fields must be strings or numbers")
    else:
        return item

def _fix_result_entry(item):
    if item == UNDEF_TXT:
        return None
    else:
        return item


class OesTxn(object):

    def __init__ (self, impl):
        self.__impl = impl

    @property
    def status(self):
        s = self.__impl.status()
        if s == oescriptimpl.ACTIVE:
            return "A"
        if s == oescriptimpl.COMMITTED:
            return "C"
        if s == oescriptimpl.ROLLEDBACK:
            return "R"
        raise Exception("invalid txn status")

    @property
    def tid(self):
        return self.__impl.tid();

#    @property
#    def exptime(self):
#        return datetime.datetime.fromtimestamp(self.__impl.exptime()//1000)

    def commit(self):
        self.__impl.commit();

    def rollback(self):
        self.__impl.rollback();


class OesLease(object):

    def __init__ (self, impl):
        self.__impl = impl

    @property
    def lid(self):
        return self.__impl.lid();

    @property
    def exptime(self):
        return datetime.datetime.fromtimestamp(self.__impl.exptime()//1000)

    def cancel(self):
        self.__impl.cancel();


class OesNet(object):

    def __init__ (self, dispatcher, use_ssl, cert, pkey):
        self.__impl = oescriptimpl.OesNet(dispatcher._OesDispatcher__impl, use_ssl, cert, pkey)


class OesServer(object):

    def __init__ (self, dispatcher, netspec, net, store):
        self.__impl = oescriptimpl.OesServer(
            dispatcher._OesDispatcher__impl,
            netspec,
            net._OesNet__impl,
            store._OesStore__impl)

    def addLoginModule(self, m):
        lm = m.loginmod
        self.__impl.addLoginModule(lm)

    def addKernel(self, extentname, k):
        self.__impl.addKernel(extentname, k._OesKernel__impl)


class OesSigHandler(object):

    def __init__(self, file_name = "mysigfile.pid", appname = "oescript", loglvl = "info"):
        self.__impl = oescriptimpl.OesSigHandler(file_name, appname, loglvl)


class OesStore(object):

    def __init__ (self, persistence_level, dir_name, is_threaded):
        self.__impl = oescriptimpl.OesStore(persistence_level, dir_name, is_threaded)


class OesThreadDispatcher(object):

    def __init__ (self, nthreads):
        self.__impl = tdispatcher = oescriptimpl.OesThreadDispatcher(nthreads)

    def start(self):
        self.__impl.start()

    def stop(self):
        self.__impl.stop()


class OesDispatcher(object):

    def __init__ (self, is_async, name):
        self.__impl = oescriptimpl.OesDispatcher(is_async, name)

    def start(self):
        self.__impl.start()

    def stop(self):
        self.__impl.stop()

class OesLoginModule(object):

    def __init__ (self):
        self.loginmod = None

class OesBuiltInLoginModule(OesLoginModule):

    def __init__ (self):
        self.__impl = oescriptimpl.OesBuiltInLoginModule()
        self.loginmod = self.__impl.getModule()

    def addAccount(self, gname, uname, pwd):
        rc = self.__impl.addAccount(gname, uname, pwd)
        if rc:
            raise Exception("can not add account")

    def addPrivilege(self, ename, gname, mtemplate):
        rc = self.__impl.addPrivilege(ename, gname, mtemplate)
        if rc:
            raise Exception("can not process privilege template: %s" % mtemplate)


class OesPamLoginModule(OesLoginModule):

    def __init__ (self):
        #todo:
        self.__impl = None
        self.loginmod = None

