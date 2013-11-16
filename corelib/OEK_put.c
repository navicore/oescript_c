/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "oe_common.h"
#include <string.h>
#include <stdio.h>
#include "OeStore.h"
#include "OeStoreHolder.h"
#include "OEK_impl.h"
#include "oescript_core_resources.h"
#include "OekMsg.h"
#include "oec_log.h"

#define T OEK

static void _sput( T _this_, OEK_Context *context ) {

    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    OeStore store = client->store;

    Arena_T arena = context->arena;

    OekMsg msg = context->cmd_msg;
    oe_id corid = OekMsg_get_cid(msg);
    oe_id txnid = OekMsg_get_tid(msg);
    PutCmd cmd = OekMsg_get_put_cmd(msg);

    //build the db entry
    DataObject values = PutCmd_get_data(cmd);
    assert(values);
    OeStoreHolder holder = OeStoreHolder_new_with_data(arena, values);

    OeStoreHolder_set_txn_id           (holder, txnid);
    oe_time exptime;
    oe_time dur = PutCmd_get_dur(cmd);
    if (dur == OEC_FOREVER) {
        exptime = OEC_FOREVER;
    } else {
        exptime = oec_get_time_milliseconds() + dur;
    }
    OeStoreHolder_set_lease_expire_time(holder, exptime);
    OeStoreHolder_set_lock             (holder, txnid == 0 ? OESTORE_UNLOCKED : OESTORE_WRITE_LOCK );
    OeStoreHolder_set_tuple            (holder, PutCmd_is_tuple(cmd));

    int ret;
    for ( int i = 0 ; i < OEK_MAX_DB_DEADLOCKS ; i++ ) {
        ret = OeStore_put(store, holder, arena);
        if ( ret != OE_STORE_PLEASE_RETRY ) break;
        OE_DLOG(NULL,OEK_STR_PUT_DL_RETRY, i + 1);
    }

    if ( ret ) {

        OE_ERR( NULL, "%s: %s", OEK_STR_PUT_FAILED, OeStore_strerror(store, ret) );

        _send_error(context, 104, OEK_STR_CAN_NOT_STORE);

    } else {

        OekMsg res_msg = OekMsg_new(PUT_RES, arena);
        PutRes res = OekMsg_get_put_res(res_msg);
        OekMsg_set_cid(res_msg, corid);
        OekMsg_set_tid(res_msg, txnid);
        //PutRes_set_expiretime(res, OeStoreHolder_get_lease_expire_time( holder ) );
        PutRes_set_lease_id(res, OeStoreHolder_get_lease_id(holder) );
        context->res_msgs = Oec_AList_list(arena, res_msg, NULL);
        _default_write_to_client(context);
    }
}

static void _store_holder( void *contextp) {
    OEK_Context *context = (OEK_Context *) contextp;
    _sput(context->kernel, context);
}

void (_oek_impl_put)( T _this_, OEK_Context *context ) {

    //context->kernel = _this_;
    assert(context->kernel);
    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    Oe_Thread_Dispatcher_exec( client->db_dispatcher, _store_holder, context );
}

#undef T

