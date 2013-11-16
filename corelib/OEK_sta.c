/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include <config.h>
#include <string.h>
#include <stdio.h>
#include "OeStore.h"
#include "OEK_impl.h"
#include "oescript_core_resources.h"
#include "OekMsg.h"
#include <unistd.h>

#define T OEK

static void (_ssta)( T _this_, OEK_Context *context );

static void _sta_async(void *contextp) {
    OEK_Context *context = (OEK_Context *) contextp;
    assert(context->kernel);
    T _this_ = context->kernel;
    _ssta( _this_, context );
}

void (_oek_impl_sta)( T _this_, OEK_Context *context ) {

    //context->kernel = _this_;
    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    Oe_Thread_Dispatcher_exec( client->db_dispatcher, _sta_async, context );
}

static void (_ssta)( T _this_, OEK_Context *context ) {

    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    OeStore store = client->store;

    Arena_T arena = context->arena;

    OekMsg msg = context->cmd_msg;
    oe_id corid = OekMsg_get_cid(msg);
    StaCmd cmd = OekMsg_get_sta_cmd(msg);
    oe_dur duration = StaCmd_get_dur(cmd);
    //bool rollback_on_disconnect = StaCmd_get_rollback_on_disconnect(cmd);

    //q: whose job is it to overide dur?

    OESTORE_TXN oetxn;
    int ret;
    for ( int i = 0 ; i < OEK_MAX_DB_DEADLOCKS ; i++ ) {
        ret = OeStore_start_txn(store, duration, &oetxn, arena);
        if ( ret != OE_STORE_PLEASE_RETRY ) break;
        OE_DLOG(NULL,OEK_STR_STA_DL_RETRY, i + 1);
    }

    if ( ret ) {
        OE_ERR( NULL, "%s: %s", OEK_STR_STA_FAILED, OeStore_strerror(store, ret) );
        _send_error(context, 109, OEK_STR_CAN_NOT_STA);
    } else {
        OekMsg res_msg = OekMsg_new(STA_RES, arena);
        StaRes res = OekMsg_get_sta_res(res_msg);
        OekMsg_set_cid(res_msg, corid);
        OekMsg_set_tid(res_msg, oetxn.tid);
        //StaRes_set_expiretime(res, oetxn.expire_time);
        context->res_msgs = Oec_AList_list(arena, res_msg, NULL);
        _default_write_to_client(context);
    }
}

#undef T

