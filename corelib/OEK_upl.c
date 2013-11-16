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

static void (_supl)( T _this_, OEK_Context *context ) {

    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    OeStore store = client->store;

    Arena_T arena = context->arena;

    OekMsg msg = context->cmd_msg;
    oe_id corid = OekMsg_get_cid(msg);
    oe_id txnid = OekMsg_get_tid(msg);
    UplCmd cmd = OekMsg_get_upl_cmd(msg);

    oe_id duration = UplCmd_get_dur(cmd);
    oe_time expire_time = oec_get_time_milliseconds() + duration;
    oe_id lease_id = UplCmd_get_lid(cmd);

    int ret;
    for ( int i = 0 ; i < OEK_MAX_DB_DEADLOCKS ; i++ ) {
        ret = OeStore_update_lease(store, lease_id, expire_time, arena);
        if ( ret != OE_STORE_PLEASE_RETRY ) break;
        OE_DLOG(NULL,OEK_STR_UPL_DL_RETRY, i + 1);
    }

    if ( ret == OE_STORE_ITEM_NOTFOUND ) {
        _send_error(context, 108, OEK_STR_LEASE_NOT_FOUND);

        return;

    } else if ( ret ) {

        OE_ERR( NULL, "%s: %s", OEK_STR_UPL_FAILED, OeStore_strerror(store, ret));

        _send_error(context, 111, OEK_STR_LEASE_UPDATE_FAILED);

        return;
    }

    OekMsg res_msg = OekMsg_new(UPL_RES, arena);
    UplRes res = OekMsg_get_upl_res(res_msg);
    OekMsg_set_cid(res_msg, corid);
    OekMsg_set_tid(res_msg, txnid);
    UplRes_set_expiretime(res, expire_time);
    context->res_msgs = Oec_AList_list(arena, res_msg, NULL);
    _default_write_to_client(context);

    return;
}

static void _upl_async(void *contextp) {
    OEK_Context *context = (OEK_Context *) contextp;
    T _this_ = context->kernel;
    _supl( _this_, context );
}

void (_oek_impl_upl)( T _this_, OEK_Context *context ) {

    //context->kernel = _this_;
    assert(context->kernel);
    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    Oe_Thread_Dispatcher_exec( client->db_dispatcher, _upl_async, context );
}

#undef T

