/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

%module oescriptimpl

%{
#include <config.h>
#include <assert.h>
#include <string.h>
#include "json_msg_factory.h"
#include "DataObject.h"
#include "DataObjectList.h"
#include "SignalHandler.h"
#include "Oed_Dispatcher.h"
#include "Oe_Thread_Dispatcher.h"
#include "OeNet.h"
#include "OeNetDefaultFactory.h"
#include "OeStore.h"
#include "OeStoreDefaultFactory.h"
#include "OEKI.h"
#include "OEK.h"
#include "OepFactory.h"
#include "OepServer.h"
#include "OepClient.h"
#include "mem.h"
#include "arena.h"
#include "oe_common.h"
#include "Oec_Future.h"
#include "LoginModule.h"
#include "BuiltInLoginModule.h"
//#ifdef OE_USE_PAM
#include "PamLoginModule.h"
//#endif
#include "OeScript.h"
%}

//todo: a swig macro that knows if the target supports cstring.i
#ifdef SWIGPERL
    #define CSTRING_I_SUPPORTED 1
#else
    #ifdef SWIGPYTHON
        #define CSTRING_I_SUPPORTED 1
    #else
        #ifdef SWIGRUBY
            #define CSTRING_I_SUPPORTED 1
        #endif
    #endif
#endif


%exception {
    oe_scalar err;
    clear_exception();
    $action
    if ((err = check_exception())) {
#if defined (SWIGPERL)
        croak(err);
       //return NULL;
#elif defined (SWIGPYTHON)
       PyErr_SetString(PyExc_RuntimeError, err);
       return NULL;
#elif defined (SWIGRUBY)
       void *runerror = rb_define_class("OeScriptRuntimeError", rb_eStandardError);
       rb_raise(runerror, err);
       return NULL;
#elif defined (SWIGLUA)
       return luaL_error(L, err);
#else
       OE_ERR( NULL, "... swig exception: %s\n", err)
       assert(false);
       return err;
#endif
    }
}


enum OES_TXN_STATUS {
    ACTIVE,
    ROLLEDBACK,
    COMMITTED
};

#ifdef CSTRING_I_SUPPORTED
%include "cstring.i"
#endif
typedef struct {
    %extend {
        OesDataObject() {
            Arena_T arena;
            arena = Arena_new();
            DataObject slist;
            slist = DataObject_new(arena);
            OesDataObject *holder;
            holder = Mem_calloc(1, sizeof(OesDataObject), __FILE__, __LINE__);
            holder->obj = slist;
            holder->arena = arena;
            holder->free_arena = true;
            return holder;
        }
        ~OesDataObject() {
            if ($self->arena && $self->free_arena) {
                Arena_dispose(&$self->arena);
            }
            Mem_free($self, __FILE__, __LINE__);
        }
        int get_nattrs() {
            return DataObject_get_nattrs($self->obj);
        }
        void add_attr(char *entry) {
            Arena_T arena;
            arena = $self->arena;
            assert(arena);
            int len = strlen(entry);
            char *new_oejcl_str;
            new_oejcl_str = Arena_alloc(arena, len + 1, __FILE__, __LINE__);
            new_oejcl_str[len] = NULL;
            strncpy(new_oejcl_str, entry, len);
            DataObject_add_attr($self->obj, (oe_scalar) new_oejcl_str);
        }
        void set_nbytes(size_t nbytes) {
            DataObject_set_nbytes($self->obj, nbytes);
        }
        int get_nbytes() {
            return DataObject_get_nbytes($self->obj);
        }
        OesIterator *iter() {
            assert($self->obj);
            OesIterator *holder;
            holder = Mem_alloc(sizeof *holder, __FILE__, __LINE__);
            holder->obj = DataObject_iterator($self->obj, false);
            return holder;
        }
        void set_bytes(char *bytes) {
            int len =  DataObject_get_nbytes($self->obj);
            assert(len);
            Arena_T arena;
            arena = $self->arena;
            char *new_oejcl_str;
            new_oejcl_str = Arena_alloc(arena, len + 1, __FILE__, __LINE__);
            new_oejcl_str[len] = NULL;
            memcpy(new_oejcl_str, bytes, len);
            DataObject_set_bytes($self->obj, new_oejcl_str);
        }
#ifdef CSTRING_I_SUPPORTED
        //%cstring_output_allocate_size(char **outx, int *sz, free(*$1)) get_rawbytes;
        %cstring_output_allocate_size(char **outx, int *sz, free(*$1));
        void get_bytes(char **outx, int *sz) {
            //use this for target langs that do impl cstrings.i and
            //can marshall to a any binary format
            int len = DataObject_get_nbytes($self->obj);
            *sz = len;
            if (len) {
                char *bytes = DataObject_get_bytes($self->obj);
                char *new_oejcl_str;
                new_oejcl_str = Mem_alloc(len + 1, __FILE__, __LINE__);
                new_oejcl_str[len] = NULL;
                memcpy(new_oejcl_str, bytes, len);
                *outx = new_oejcl_str;
            }
        }
#else
        char *get_bytes() {
            //use this for target langs that don't impl cstrings.i but
            //can marshall to a null terminated char* string 
            int len = DataObject_get_nbytes($self->obj);
            char *new_oejcl_str = Mem_alloc(len + 1, __FILE__, __LINE__);
            if (len) {
                char *entry = DataObject_get_bytes($self->obj);
                memcpy(new_oejcl_str, entry, len);
            }
            new_oejcl_str[len] = NULL;
            return new_oejcl_str;
        }
#endif

    }
} OesDataObject;

typedef struct {
    %extend {
        OesSigHandler(const char *filename,
                      const char *appname,
                      const char *loglvl) {
            SignalHandler h;
            char *lfilename = Mem_alloc(strlen(filename) + 1, __FILE__, __LINE__);
            memcpy(lfilename, filename, strlen(filename) + 1);
            char *lloglvl = Mem_alloc(strlen(loglvl) + 1, __FILE__, __LINE__);
            memcpy(lloglvl, loglvl, strlen(loglvl) + 1);
            char *lappname = Mem_alloc(strlen(appname) + 1, __FILE__, __LINE__);
            memcpy(lappname, appname, strlen(appname) + 1);
            h = SignalHandler_new(lfilename, lappname, lloglvl);
            OesSigHandler *holder;
            holder = Mem_alloc(sizeof(OesSigHandler), __FILE__, __LINE__);
            holder->obj = h;
            return holder;
        }
        ~OesSigHandler() {
            SignalHandler_shutdown($self->obj);
            SignalHandler_free(&$self->obj);
            Mem_free($self, __FILE__, __LINE__);
        }
        //
        //note, swig 2.0.4 breaks the below 'add' function
        //  overloading.  2.0.2 and earlier work fine.
        //
        //leaks (manageme will live for the duration of the hosting env)
        void add( OesDispatcher *manageme ) {
            SignalHandler_manage($self->obj, 
                                 Oed_Dispatcher_stop, 
                                 Oed_Dispatcher_free, 
                                 Oed_Dispatcher_stats, 
                                 manageme->obj);
            manageme->managed = true;
        }
        void add( OesThreadDispatcher *manageme ) {
            SignalHandler_manage($self->obj, 
                                 Oe_Thread_Dispatcher_stop,
                                 Oe_Thread_Dispatcher_free,
                                 Oe_Thread_Dispatcher_stats,
                                 manageme->obj);
            manageme->managed = true;
        }
        void add( OesStore *manageme ) {
            SignalHandler_manage($self->obj, 
                                 OeStore_stop,
                                 OeStore_free,
                                 OeStore_stats,
                                 manageme->obj);
            manageme->managed = true;
        }
        void add( OesServer *manageme ) {
            SignalHandler_manage($self->obj, 
                                 NULL,
                                 OepServer_free,
                                 OepServer_stats,
                                 manageme->obj);
            manageme->managed = true;
        }
        void add( OesKernel *manageme ) {
            SignalHandler_manage($self->obj, 
                                 OEK_stop, 
                                 OEK_free, 
                                 OEK_stats, 
                                 manageme->obj);
            manageme->managed = true;
        }
        void add( OesNet *manageme ) {
            SignalHandler_manage($self->obj, 
                                 OeNet_stop,
                                 OeNet_free,
                                 OeNet_stats,
                                 manageme->obj);
            manageme->managed = true;
        }
    }
} OesSigHandler;

typedef struct {
    %extend {
        OesDispatcher(size_t nthreads, const char *name) {
            OesDispatcher *holder;
            holder = Mem_calloc(1, sizeof(OesDispatcher), __FILE__, __LINE__);
            holder->obj = Oed_Dispatcher_new(nthreads, name);
            holder->managed = false;
            return holder;
        }
        ~OesDispatcher() {
            if (!$self->managed) {
                Oed_Dispatcher_stop($self->obj);
                Oed_Dispatcher_free(&$self->obj);
            }
            Mem_free($self, __FILE__, __LINE__);
        }
        void start() {
            Oed_Dispatcher_start($self->obj);
        }
    }
} OesDispatcher;

typedef struct {
    %extend {
        OesThreadDispatcher(size_t nthreads) {
            OesThreadDispatcher *holder;
            holder = Mem_calloc(1, sizeof(OesThreadDispatcher), __FILE__, __LINE__);
            holder->obj = Oe_Thread_Dispatcher_new(nthreads);
            holder->managed = false;
            return holder;
        }
        ~OesThreadDispatcher() {
            if (!$self->managed) {
                Oe_Thread_Dispatcher_stop($self->obj);
                Oe_Thread_Dispatcher_free(&$self->obj);
            }
            Mem_free($self, __FILE__, __LINE__);
        }
        void start() {
            Oe_Thread_Dispatcher_start($self->obj);
        }
    }
} OesThreadDispatcher;

typedef struct {
    %extend {
        OesStore(int persist_level, const char *homedir, bool threaded) {
            OesStore *holder;
            holder = Mem_calloc(1, sizeof(OesStore), __FILE__, __LINE__);
            holder->obj = OeStoreDefaultFactory_create(persist_level,
                                                       homedir, threaded);
            holder->managed = false;
            return holder;
        }
        ~OesStore() {
            if (!$self->managed) {
                OeStore_stop($self->obj);
                OeStore_free(&$self->obj);
            }
            Mem_free($self, __FILE__, __LINE__);
        }
    }
} OesStore;

typedef struct {
    %extend {
        OesNet(OesDispatcher *d, bool use_ssl, char *cert, char *pkey) {
            OesNet *holder;
            holder = Mem_calloc(1, sizeof(OesNet), __FILE__, __LINE__);
            if (use_ssl) {
                holder->obj = OeNetDefaultSslFactory_create(d->obj, cert, pkey);
            } else {
                holder->obj = OeNetDefaultFactory_create(d->obj);
            }
            holder->managed = false;
            return holder;
        }
        ~OesNet() {
            if (!$self->managed) {
                OeNet_stop($self->obj);
                OeNet_free(&$self->obj);
            }
            Mem_free($self, __FILE__, __LINE__);
        }
        void setName(char *name) {
            OeNet_set_name($self->obj, (oe_scalar) name);
        }
        char *getName() {
            return (char *) OeNet_get_name($self->obj);
        }
    }
} OesNet;

typedef struct {
    %extend {
        ~OesLoginModule() {
            if (!$self->managed) {
                LoginModule_free(&$self->obj);
            }
            Mem_free($self, __FILE__, __LINE__);
        }
    }
} OesLoginModule;

typedef struct {
    %extend {
        OesBuiltInLoginModule() {
            OesBuiltInLoginModule *holder;
            holder = Mem_calloc(1, sizeof(OesBuiltInLoginModule), __FILE__, __LINE__);
            holder->obj = BuiltInLoginModule_new();
            holder->managed = false;
            return holder;
        }
        ~OesBuiltInLoginModule() {
            //impl'll get cleaned up by LoginModule
            Mem_free($self, __FILE__, __LINE__);
        }
        int addAccount(char *groupname, char *username,  char *pwd) {
            BuiltInLoginModule_add_account($self->obj, groupname, username, pwd);
            return 0;
        }
        int addPrivilege(char *extentname, char *groupname, char *mtemplate) {
            OekMsg msg = 0;
            int rc = json_msg_create(&msg, mtemplate,
                                     BuiltInLoginModule_get_arena($self->obj));
            if (rc) {
                //throw_exception(PARSE_PRIV_ERR);
                char *big_em = oec_str_concat(NULL, 2, PARSE_PRIV_ERR, mtemplate); //leaks
                throw_exception(big_em);
                return 1;
            }
            BuiltInLoginModule_add_privilege($self->obj, extentname, groupname, msg);
            return 0;
        }
        OesLoginModule *getModule() {
            OesLoginModule *holder;
            holder = Mem_alloc(sizeof *holder, __FILE__, __LINE__);
            holder->obj = BuiltInLoginModule_new_module($self->obj);
            return holder;
        }
    }
} OesBuiltInLoginModule;

//#ifdef OE_USE_PAM
typedef struct {
    %extend {
        OesPamLoginModule(char *pamdomain, char *defgroup) {
            OesPamLoginModule *holder;
            holder = Mem_calloc(1, sizeof(OesPamLoginModule), __FILE__, __LINE__);
            holder->obj = PamLoginModule_new((oe_scalar) pamdomain, (oe_scalar) defgroup);
            holder->managed = false;
            return holder;
        }
        ~OesPamLoginModule() {
            //impl'll get cleaned up by LoginModule
            Mem_free($self, __FILE__, __LINE__);
        }
        /*
        int addAccount(char *groupname, char *username,  char *pwd) {
            PamLoginModule_add_account($self->obj, groupname, username, pwd);
            return 0;
        }
        */
        int addPrivilege(char *extentname, char *groupname, char *mtemplate) {
            OekMsg msg = 0;
            int rc = json_msg_create(&msg, mtemplate,
                                     PamLoginModule_get_arena($self->obj));
            if (rc) {
                //throw_exception(PARSE_PRIV_PAM_ERR);
                char *big_em = oec_str_concat(NULL, 2, PARSE_PRIV_PAM_ERR, mtemplate); //leaks
                throw_exception(big_em);
                return 1;
            }
            PamLoginModule_add_privilege($self->obj, extentname, groupname, msg);
            return 0;
        }
        OesLoginModule *getModule() {
            OesLoginModule *holder;
            holder = Mem_alloc(sizeof *holder, __FILE__, __LINE__);
            holder->obj = PamLoginModule_new_module($self->obj);
            return holder;
        }
    }
} OesPamLoginModule;
//#endif

typedef struct {
    %extend {
        OesServer(OesDispatcher *dispatcher,
                  char *spec, OesNet *net, OesStore *store) {
            OesServer *holder;
            holder = Mem_calloc(1, sizeof(OesServer), __FILE__, __LINE__);
            holder->obj = OepFactory_new_server(dispatcher->obj, spec, net->obj, store->obj);
            holder->managed = false;
            return holder;
        }
        ~OesServer() {
            if (!$self->managed) {
                OepServer_free(&$self->obj);
            }
            Mem_free($self, __FILE__, __LINE__);
        }
        void addLoginModule(OesLoginModule *m) {
            OepServer_add_login_module($self->obj, m->obj);
        }
        void addKernel(char *extentname, OesKernel *kernel) {
            OepServer_add_kernel($self->obj, extentname, kernel->obj);
        }
    }
} OesServer;

////////////////////////////////////////////////////////////////
///                         responses                        ///
////////////////////////////////////////////////////////////////
typedef struct {
    %extend {
        OesDataObjectList() {
            return OesKernel_impl_createOesDataObjectList(Arena_new(), NULL, NULL);
        }
        ~OesDataObjectList() {
            if ($self->f) {
                Oec_Future_free(&$self->f);
            }
            if ($self->arena) {
                Arena_dispose(&$self->arena);
            }
            Mem_free($self, __FILE__, __LINE__);
        }
        int length() {
            if ($self->f) {
                Oec_Future_get($self->f, OEC_FOREVER); //just to make sure it is done
            }
            return DataObjectList_length($self->obj);
        }
        OesListIterator *iter() {
            OesListIterator *holder;
            holder = Mem_alloc(sizeof *holder, __FILE__, __LINE__);
            holder->l = $self->obj;
            holder->f = $self->f;
            holder->obj = NULL;
            return holder;
        }
        OesDataObject *newDataObject() {
            if ($self->f) {
                Oec_Future_get($self->f, OEC_FOREVER); //just to make sure it is done
            }
            OesDataObject *holder;
            holder = Mem_calloc(1, sizeof *holder, __FILE__, __LINE__);
            holder->obj = DataObjectList_new_data_object($self->obj);
            holder->arena = $self->arena;
            return holder;
        }
    }
} OesDataObjectList;

typedef struct {
    %extend {
        ~OesIterator() {
            Iterator_free($self->obj);
            Mem_free($self, __FILE__, __LINE__);
        }
        char *nextItem() {
            assert($self->obj);
            char *nl = Iterator_next($self->obj);
            if (nl == NULL) {
                return NULL;
            }
            int len = strlen(nl);
            char *newattr = Mem_alloc(len + 1, __FILE__, __LINE__);
            strncpy(newattr, nl, len);
            newattr[len] = NULL;
            return newattr;
        }
        bool hasMore() {
            assert($self->obj);
            return Iterator_hasMore($self->obj);
        }
    }
} OesIterator;

typedef struct {
    %extend {
        ~OesListIterator() {
            if ($self->obj) Iterator_free($self->obj);
            Mem_free($self, __FILE__, __LINE__);
        }
        OesDataObject *nextItem() {
            assert($self->l);
            if ($self->f) Oec_Future_get($self->f, OEC_FOREVER);
            if (!$self->obj) $self->obj = DataObjectList_iterator($self->l, false);
            DataObject nl = Iterator_next($self->obj);
            if (nl == NULL) {
                return NULL;
            }
            OesDataObject *holder;
            holder = Mem_calloc(1, sizeof(OesDataObject), __FILE__, __LINE__);
            holder->obj = nl;
            return holder;
        }
        bool hasMore() {
            assert($self->l);
            if ($self->f) Oec_Future_get($self->f, OEC_FOREVER);
            if (!$self->obj) $self->obj = DataObjectList_iterator($self->l, false);
            return Iterator_hasMore($self->obj);
        }
    }
} OesListIterator;

typedef struct {
    %extend {
        ~OesLease() {
            Oec_Future_get($self->f, OEC_FOREVER);
            Oec_Future_free(&$self->f);
            Mem_free($self, __FILE__, __LINE__);
        }
        long lid() {
            Oec_Future_get($self->f, OEC_FOREVER);
            return $self->lid;
        }
        /*
        long long exptime() {
            Oec_Future_get($self->f, OEC_FOREVER);
            return $self->exptime;
        }
        */
        void cancel() {
            Oec_Future_get($self->f, OEC_FOREVER);
            OesKernel_impl_upl_cancel($self);
        }
    }
} OesLease;

typedef struct {
    %extend {
        ~OesTxn() {
            Oec_Future_get($self->f, OEC_FOREVER);
            Oec_CountdownLatch_free($self->enlisted);
            Oec_CountdownLatch_free($self->enlisted_counter);
            Oec_Future_free(&$self->f);
            Mem_free($self, __FILE__, __LINE__);
        }
        long tid() {
            Oec_Future_get($self->f, OEC_FOREVER);
            return $self->tid;
        }
        enum OES_TXN_STATUS status() {
            Oec_Future_get($self->f, OEC_FOREVER);
            return $self->status;
        }
        void commit() {
            OesKernel_impl_commit($self);
        }
        void rollback() {
            OesKernel_impl_rollback($self);
        }
    }
} OesTxn;


//////////////////////////////////////////////////////////
///                         api                        ///
//////////////////////////////////////////////////////////

typedef struct {
    %extend {
        OesKernel(OesDispatcher *dispatcher,
                  OesThreadDispatcher *tdispatcher,
                  OesStore *store,
                  char *netspec, OesNet *net, char *username, char *pwd) {

            OesKernel *holder;
            holder = Mem_calloc(1, sizeof(OesKernel), __FILE__, __LINE__);
            if (!netspec || strlen(netspec) == 0) {
                assert(dispatcher);
                assert(tdispatcher);
                assert(store);
                holder->obj = OEK_default_factory_new(dispatcher->obj,
                                                      tdispatcher->obj, store->obj);
            } else {
                assert(dispatcher); assert(net); assert(username); assert(pwd); assert(netspec);
                OE_DLOG( NULL, "OeScript:OesKernel remote connect user %s to %s\n",
                         username, netspec); // i18n
                OepClient cl = OepFactory_new_client(netspec, 
                                                     dispatcher->obj, net->obj,
                                                     username, pwd);
                if (cl == NULL) {
                    throw_exception("can not connect"); //i18n
                    return NULL;
                }
                holder->obj = OepClient_new_kernel(cl);
            }
            return holder;
        }
        ~OesKernel() {
            if (!$self->managed) {
                OEK_stop($self->obj);
                OEK_free(&$self->obj);
            }
            /*
            OEK_stop($self->obj);
            if ($self->shutdown_cb) {
                //the shutdown is done externally because the kernel isn't smart
                // enough to know the sequence of stopping itself and its own parts.
                // this is usually a ref to SignalHandler shutdown cmd.
                $self->shutdown_cb($self->shutdown_cb_arg);
            } else {
                OEK_free(&$self->obj);
            }
            */
            Mem_free($self, __FILE__, __LINE__);
        }

        OesLease *write(OesDataObject *slist, long dur, bool is_tuple) {
            return OesKernel_impl_write($self, slist, (oe_time) dur, is_tuple);
        }

        OesTxn *begin(long dur) {
            return OesKernel_impl_begin($self, (oe_time) dur);
        }

        void setCurrent(OesTxn *txn_holder) {
            $self->currentTxn = txn_holder;
        }

        OesDataObjectList *read(OesDataObjectList *data, long timeout,
                                bool is_tuple, int max_responses,
                                bool return_attrs, bool return_bytes) {
            return OesKernel_impl_read($self, data, (oe_time) timeout,
                                       is_tuple, max_responses,
                                       return_attrs, return_bytes);
        }

        OesDataObjectList *take(OesDataObjectList *data, long timeout,
                                bool is_tuple, int max_responses,
                                bool return_attrs, bool return_bytes) {
            return OesKernel_impl_take($self, data, (oe_time) timeout,
                                       is_tuple, max_responses,
                                       return_attrs, return_bytes);
        }
/*
        void set_sig_handler(OesSigHandler *h) {
            assert(h);
            $self->shutdown_cb_arg = h->obj;
            $self->shutdown_cb = SignalHandler_shutdown;
        }
*/
    }

} OesKernel;

typedef struct {
    %extend {
        OesJsonKernel(OesKernel *kernel) {

            OesJsonKernel *holder;
            holder = Mem_calloc(1, sizeof(OesJsonKernel), __FILE__, __LINE__);
            holder->obj = JsonKernel_new(kernel->obj);
            return holder;
        }
        ~OesJsonKernel() {
            if (!$self->managed) {
                JsonKernel_free(&$self->obj);
            }
            Mem_free($self, __FILE__, __LINE__);
        }

        OesFuture *exec(char *json) {

            Oec_Future f = Oec_Future_new();

            JsonKernel_exec($self->obj, json, OesKernel_impl_json_promise, f);

            OesFuture *folder;
            folder = Mem_calloc(1, sizeof(OesFuture), __FILE__, __LINE__);
            folder->obj = f;
            return folder;
        }
    }
} OesJsonKernel;

typedef struct {
    %extend {
        ~OesFuture() {
            Oec_Future_free(&$self->obj);
            Mem_free($self, __FILE__, __LINE__);
        }

        char *getString(long timeout) {

            Oec_Future f = $self->obj;

            return (char *) Oec_Future_get(f, timeout);
        }
    }

} OesFuture;

