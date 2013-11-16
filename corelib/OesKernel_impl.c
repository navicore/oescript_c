/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "OEKI.h"
#include "OekMsg.h"
#include "OeStore.h"
#include "OepClient.h"
#include "oec_threads.h"
#include "oescript_core_resources.h"
#include "OEK_default_factory.h"
#include "OEK.h"
#include "Oec_Future.h"
#include "OeScript.h"
#include "Oes_resources.h"

static OEK_Context *_new_oek_context(Arena_T arena) {
    OEK_Context *context;
    context = Arena_calloc(arena, 1, sizeof *context, __FILE__, __LINE__);
    context->arena = arena;
    return context;
}
static OEK_Context *_new_get_cmd_context(Arena_T arena) {
    OEK_Context *context = _new_oek_context(arena);
    OekMsg msg = OekMsg_new(GET_CMD, arena);
    GetCmd cmd = OekMsg_get_get_cmd(msg);
    GetCmd_set_max_responses(cmd, 1);
    context->cmd_msg = msg;
    return context;
}
static OEK_Context *_new_put_cmd_context(Arena_T arena) {
    OEK_Context *context = _new_oek_context(arena);
    OekMsg msg = OekMsg_new(PUT_CMD, arena);
    context->cmd_msg = msg;
    return context;
}
static OEK_Context *_new_sta_cmd_context(Arena_T arena) {
    OEK_Context *context = _new_oek_context(arena);
    OekMsg msg = OekMsg_new(STA_CMD, arena);
    context->cmd_msg = msg;
    return context;
}
static OEK_Context *_new_upt_cmd_context(Arena_T arena) {
    OEK_Context *context = _new_oek_context(arena);
    OekMsg msg = OekMsg_new(UPT_CMD, arena);
    context->cmd_msg = msg;
    return context;
}
static OEK_Context *_new_upl_cmd_context(Arena_T arena) {
    OEK_Context *context = _new_oek_context(arena);
    OekMsg msg = OekMsg_new(UPL_CMD, arena);
    context->cmd_msg = msg;
    return context;
}

static void _oes_put_promise_cb(OEK_Context *context) {
    //ejs fixme json flips this latch but jcl does not
    assert(context);
    assert(context->arena);

    OesLease *lh = context->p_context;
    OesKernel *kernel_holder = lh->kernel_holder;
    if (context->err_msg) {
        OekErr em = OekMsg_get_err(context->err_msg);
        throw_exception(OekErr_get_message(em));
    } else {
        OekMsg *res_msg_array = (OekMsg*) Oec_AList_toArray(context->res_msgs, context->arena, NULL);
        PutRes res = OekMsg_get_put_res(res_msg_array[0]);
        oe_id lid = PutRes_get_lease_id(res);
        lh->lid = lid;
    }
    if (lh->txn_holder) {
        Oec_CountdownLatch_decrement(lh->txn_holder->enlisted);
        Oec_CountdownLatch_increment(lh->txn_holder->enlisted_counter);
    }

    Oec_Future_set(lh->f, NULL); //flip latch
}

OesLease *OesKernel_impl_write(OesKernel *kernel_holder,
                               OesDataObject *slist,
                               oe_time dur,
                               bool is_tuple) {
    OesLease *holder;
    holder = Mem_calloc(1, sizeof *holder, __FILE__, __LINE__);
    Oec_Future f = Oec_Future_new();
    Arena_T arena = slist->arena;
    assert(arena); //if arena is null the user is doing
                   //multiple puts with the same
                   //strlist but this isn't supported
                   //since we're using the arena from the
                   //first request in the async txn
    slist->arena = NULL;
    //////////////////////////////////////
    //////////////////////////////////////
    /// note, you are taking the arena ///
    /// from the user's obj and using  ///
    /// that for the async txn.  the   ///
    /// user can gc the slist but the  ///
    /// underlying DataObject won't be ///
    /// freed until the async txn is   ///
    /// done.                          ///
    //////////////////////////////////////
    //////////////////////////////////////
    Oec_Future_set_arena(f, arena);
    holder->f = f;

    OEK_Context *context = _new_put_cmd_context(arena);
    context->kernel = kernel_holder->obj;
    OekMsg cmd_msg = context->cmd_msg;
    PutCmd cmd = OekMsg_get_put_cmd(cmd_msg);

    if (kernel_holder->currentTxn) {
        OesTxn *txn_holder = kernel_holder->currentTxn;
        holder->txn_holder = txn_holder;
        Oec_Future_getid(txn_holder->f, OEC_FOREVER);
        OekMsg_set_tid(cmd_msg, txn_holder->tid);
        int enlisted = Oec_CountdownLatch_increment(txn_holder->enlisted);
        assert(enlisted);
    }
    OekMsg_set_cid(cmd_msg, ++kernel_holder->lastid);

    PutCmd_set_dur(cmd, dur > 0 ? dur : OEC_FOREVER);
    PutCmd_set_data(cmd, slist->obj);
    if (is_tuple) {
        PutCmd_set_tuple(cmd);
    }

    context->write_response_cb = _oes_put_promise_cb;

    holder->kernel_holder = kernel_holder;
    context->p_context = holder;

    OEK_put(kernel_holder->obj, context);

    return holder;
}

static void _oes_get_promise_cb(OEK_Context *context) {
    assert(context);
    assert(context->arena);

    OesDataObjectList *results = context->p_context;
    OesKernel *kernel_holder = results->kernel_holder;
    bool done = false;
    int nresults = 0;
    if (context->err_msg) {
        OekErr em = OekMsg_get_err(context->err_msg);
        throw_exception(OekErr_get_message(em));
        done = true;
    } else {
        int sz = Oec_AList_length(context->res_msgs);
        OekMsg *res_msg_array =
        (OekMsg*) Oec_AList_toArray(context->res_msgs,
                                    context->arena, NULL);
        for (int i=0; i<sz; i++) {
            GetRes res = OekMsg_get_get_res(res_msg_array[i]);
            nresults = GetRes_get_nresults(res);
            if (nresults > 0) {
                DataObjectList_add(results->obj, GetRes_get_data(res));
            }
        }
        if (DataObjectList_length(results->obj) == nresults) {
            done = true;
        }
    }
    if (done) {
        GetCmd cmd = OekMsg_get_get_cmd(context->cmd_msg);
        bool take = GetCmd_remove(cmd);
        if (take && results->txn_holder) {
            Oec_CountdownLatch_decrement(results->txn_holder->enlisted);
            for (int i = 0; i < nresults; i++) {
                Oec_CountdownLatch_increment(results->txn_holder->enlisted_counter);
            }
        }

        Oec_Future_set(results->f, NULL); //flip latch
    }
}

static OesDataObjectList *_oes_get(OesKernel *kernel_holder,
                                   OesDataObjectList *values,
                                   oe_time timeout,
                                   bool destroy,
                                   bool is_tuple,
                                   int max_responses,
                                   bool return_attrs,
                                   bool return_bytes) {
    assert(kernel_holder);
    assert(values);
    assert(values->obj);
    Oec_Future f;
    f = Oec_Future_new();
    Arena_T arena = values->arena;
    if (arena) {
        values->arena = NULL;
    } else {
        arena = Arena_new(); //must be a second throw //todo: why rely make arena so soon?
    }
    assert(arena);
    Oec_Future_set_arena(f, arena);
    OesDataObjectList *holder;
    holder = OesKernel_impl_createOesDataObjectList(arena, f,
                                                    kernel_holder->currentTxn);

    OEK_Context *context = _new_get_cmd_context(arena);
    context->kernel = kernel_holder->obj;
    OekMsg cmd_msg = context->cmd_msg;
    GetCmd cmd = OekMsg_get_get_cmd(cmd_msg);

    if (kernel_holder->currentTxn) {
        OesTxn *txn_holder = kernel_holder->currentTxn;
        holder->txn_holder = txn_holder;
        Oec_Future_getid(txn_holder->f, OEC_FOREVER);
        OekMsg_set_tid(cmd_msg, txn_holder->tid);
        if (destroy) {
            int enlisted = Oec_CountdownLatch_increment(txn_holder->enlisted);
            assert(enlisted);
        }
    }
    OekMsg_set_cid(cmd_msg, ++kernel_holder->lastid);
    if (destroy) {
        GetCmd_set_remove_option(cmd);
        bool take = GetCmd_remove(cmd);
    }

    GetCmd_set_timeout(cmd, timeout);
    if (is_tuple) {
        GetCmd_set_tuple(cmd);
    }

    GetCmd_set_data_objects(cmd, values->obj);
    if (return_bytes) GetCmd_set_return_bytes(cmd, true);
    if (return_attrs) GetCmd_set_return_attrs(cmd, true);
    GetCmd_set_max_responses(cmd, max_responses);

    context->write_response_cb = _oes_get_promise_cb;
    context->expire_time = oec_get_time_milliseconds() + timeout;

    holder->kernel_holder = kernel_holder;
    context->p_context = holder;

    OEK_get(kernel_holder->obj, context);

    return holder;
}

OesDataObjectList *OesKernel_impl_read(OesKernel *kernel_holder,
                                       OesDataObjectList *values,
                                       oe_time timeout,
                                       bool is_tuple,
                                       int max_responses,
                                       bool return_attrs,
                                       bool return_bytes) {
    return _oes_get(kernel_holder,values,timeout, false, is_tuple, max_responses,
                    return_attrs, return_bytes);
}

OesDataObjectList *OesKernel_impl_take(OesKernel *kernel_holder,
                                       OesDataObjectList *values,
                                       oe_time timeout,
                                       bool is_tuple,
                                       int max_responses,
                                       bool return_attrs,
                                       bool return_bytes) {
    return _oes_get(kernel_holder,values,timeout, true, is_tuple, max_responses, 
                    return_attrs, return_bytes);
}

void OesKernel_impl_json_promise(char *json, void *future) {

    Oec_Future_set(future, json);
}

static void _oes_begin_promise_cb(OEK_Context *context) {
    assert(context);
    assert(context->arena);

    OesTxn *txn_holder = context->p_context;
    OesKernel *kernel_holder = txn_holder->kernel_holder;
    if (context->err_msg) {
        OekErr em = OekMsg_get_err(context->err_msg);
        throw_exception(OekErr_get_message(em));
    } else {
        OekMsg *res_msg_array = (OekMsg*) Oec_AList_toArray(context->res_msgs, context->arena, NULL);
        OekMsg msg = res_msg_array[0];
        txn_holder->tid = OekMsg_get_tid(msg);
        StaRes res = OekMsg_get_sta_res(msg);
        //txn_holder->exptime = StaRes_get_expiretime(res);
        txn_holder->status = ACTIVE;
    }

    Oec_Future_set(txn_holder->f, NULL); //flip latch
}

OesTxn *OesKernel_impl_begin(OesKernel *kernel_holder, oe_time dur) {
    OesTxn *holder;
    holder = Mem_alloc(sizeof(OesTxn), __FILE__, __LINE__);
    kernel_holder->currentTxn = holder;
    Oec_Future f = Oec_Future_new();
    Arena_T arena = Arena_new();
    Oec_Future_set_arena(f, arena);
    holder->f = f;
    holder->enlisted = Oec_CountdownLatch_new();
    holder->enlisted_counter = Oec_CountdownLatch_new();
    //call oek sta

    OEK_Context *context = _new_sta_cmd_context(arena);
    context->kernel = kernel_holder->obj;
    OekMsg cmd_msg = context->cmd_msg;
    OekMsg_set_cid(cmd_msg, ++kernel_holder->lastid);
    StaCmd cmd = OekMsg_get_sta_cmd(cmd_msg);

    StaCmd_set_dur(cmd, dur);

    context->write_response_cb = _oes_begin_promise_cb;

    holder->kernel_holder = kernel_holder;
    context->p_context = holder;

    OEK_sta(kernel_holder->obj, context);

    return holder;
}

static void _oes_upt_promise_cb(OEK_Context *context) {
    assert(context);
    assert(context->arena);

    OesTxn *txn_holder = context->p_context;
    OesKernel *kernel_holder = txn_holder->kernel_holder;
    if (context->err_msg) {
        OekErr em = OekMsg_get_err(context->err_msg);
        throw_exception(OekErr_get_message(em));
    } else {
        OekMsg *res_msg_array = (OekMsg*) Oec_AList_toArray(context->res_msgs, context->arena, NULL);
        OekMsg msg = res_msg_array[0];
        UptRes res = OekMsg_get_upt_res(msg);
        //txn_holder->exptime = UptRes_get_expiretime(res);
        txn_holder->status = UptRes_get_status(res) == 'C' ? COMMITTED : ROLLEDBACK;
    }

    Oec_Future_set(txn_holder->f, NULL); //flip latch
}

static void _oes_upt_txn (OesTxn *txn_holder, enum OES_TXN_STATUS status) {

    if (!txn_holder) {
        throw_exception("null txn_holder");
    }
    if (txn_holder->status != ACTIVE) {
        throw_exception("null txn is not active");
    }
    OesKernel *kernel_holder = txn_holder->kernel_holder;

    Oec_Future_getid(txn_holder->f, OEC_FOREVER); //make sure it is done
    Oec_Future_free(&txn_holder->f);
    txn_holder->f = Oec_Future_new();

    Arena_T arena = Arena_new();
    OEK_Context *context = _new_upt_cmd_context(arena);
    context->kernel = kernel_holder->obj;
    txn_holder->arena = arena;
    Oec_Future_set_arena(txn_holder->f, arena);
    OekMsg cmd_msg = context->cmd_msg;
    UptCmd cmd = OekMsg_get_upt_cmd(cmd_msg);

    OekMsg_set_cid(cmd_msg, ++kernel_holder->lastid);
    OekMsg_set_tid(cmd_msg, txn_holder->tid);
    UptCmd_set_status(cmd, status == COMMITTED ? 'C' : 'R');

    context->write_response_cb = _oes_upt_promise_cb;

    context->p_context = txn_holder;

    int gotlatch = Oec_CountdownLatch_get(txn_holder->enlisted, OEC_FOREVER);

    assert(gotlatch == 1);

    kernel_holder->currentTxn = NULL; //done

    //tell the server how many items we think we have enlisted
    int e = Oec_CountdownLatch_get_hi_water_mark(txn_holder->enlisted_counter);
    UptCmd_set_enlisted(cmd, e > 0 ? e : -1);

    OEK_upt(kernel_holder->obj, context);
}

static void _oes_upl_promise_cb(OEK_Context *context) {

    OesLease *lease_holder = context->p_context;
    Oec_Future_set(lease_holder->f, NULL); //flip latch
}

void OesKernel_impl_upl_cancel(OesLease *lease_holder) {

    OesKernel *kernel_holder = lease_holder->kernel_holder;

    Oec_Future_get(lease_holder->f, OEC_FOREVER); //make sure it is done
    Oec_Future_free(&lease_holder->f);
    lease_holder->f = Oec_Future_new();

    Arena_T arena = Arena_new();
    Oec_Future_set_arena(lease_holder->f, arena);
    OEK_Context *context = _new_upl_cmd_context(arena);
    context->kernel = kernel_holder->obj;
    OekMsg cmd_msg = context->cmd_msg;
    UplCmd cmd = OekMsg_get_upl_cmd(cmd_msg);
    UplCmd_set_dur(cmd, 0);
    UplCmd_set_lid(cmd, lease_holder->lid);

    OekMsg_set_cid(cmd_msg, ++kernel_holder->lastid);

    context->write_response_cb = _oes_upl_promise_cb;

    context->p_context = lease_holder;

    OEK_upl(kernel_holder->obj, context);
}

void OesKernel_impl_commit(OesTxn *txn_holder) {
    _oes_upt_txn(txn_holder, COMMITTED);
}

void OesKernel_impl_rollback(OesTxn *txn_holder) {
    _oes_upt_txn(txn_holder, ROLLEDBACK);
}

OesDataObjectList *OesKernel_impl_createOesDataObjectList(Arena_T arena,
                                                          Oec_Future future,
                                                          OesTxn *txn) {
    OesDataObjectList *holder;
    holder = Mem_calloc(1, sizeof *holder, __FILE__, __LINE__);
    holder->obj = DataObjectList_new(arena);
    holder->arena = arena;
    holder->f = future;
    holder->txn_holder = txn;
    return holder;
}

//
// exceptions
//
static char error_message[256];
static int error_status = 0;

void throw_exception(oe_scalar msg) {
    OE_DLOG( NULL, "throw exception: %s\n", (char *) msg)
    size_t len = strlen(msg);
	strncpy(error_message, (char *) msg, len < 256 ? len : 256);
	error_status = 1;
}

void clear_exception() {
	error_status = 0;
}
oe_scalar check_exception() {
	if (error_status) return (oe_scalar) error_message;
	else return NULL;
}

