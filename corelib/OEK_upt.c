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

OESTORE_TXN_STATUS _get_txn_status(char statuschar) {
    if ( statuschar == 'C' ) {
        return OESTORE_TXN_COMMITTED;
    } else if ( statuschar == 'R' ) {
        return OESTORE_TXN_ROLLED_BACK;
    }
    return OESTORE_TXN_ACTIVE;
}

/*
static void (_supt_query)( T _this_, OEK_Context *context) {

    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    OeStore store = client->store;

    Arena_T arena = context->arena;
    OekMsg msg = context->cmd_msg;
    oe_id corid = OekMsg_get_cid(msg);
    oe_id txnid = OekMsg_get_tid(msg);

    OESTORE_TXN_STATUS status;
    oe_time expire_time;

    int ret;
    for ( int i = 0 ; i < OEK_MAX_DB_DEADLOCKS ; i++ ) {
        ret = OeStore_query_txn(store, txnid, &expire_time, &status, arena);
        if ( ret != OE_STORE_PLEASE_RETRY ) break;
        OE_DLOG(NULL,OEK_STR_UPT_DL_RETRY, i + 1, txnid);
    }

    if (ret == OE_STORE_ITEM_NOTFOUND) {
        _send_error(context, 106, OEK_STR_TXN_NOT_FOUND);
        return;

    } else if ( ret ) {

        OE_ERR( NULL, "%s: %s", OEK_STR_UPT_FAILED, OeStore_strerror(store, ret) );

        char *msg;
        if ( ret == OESTORE_UPDATING_EXPIRED_TXN_ERR ) {
            msg = OEK_STR_TXN_EXPIRED;
        } else {
            msg = OEK_STR_TXN_UPDATE_FAILED;
            OE_DLOG(NULL,OEK_STR_TXN_UPDATE_ERR, msg, txnid, OeStore_strerror(store, ret));
        }
        _send_error(context, 107, msg);
        return;
    }

    OekMsg res_msg = OekMsg_new(UPT_RES, arena);
    UptRes res = OekMsg_get_upt_res(res_msg);
    OekMsg_set_cid(res_msg, corid);
    OekMsg_set_tid(res_msg, txnid);
    UptRes_set_expiretime(res, expire_time);
    if ( status == OESTORE_TXN_COMMITTED ) {
        UptRes_set_status(res, OETXN_COMMITTED);
    } else if ( status == OESTORE_TXN_ROLLED_BACK  ) {
        UptRes_set_status(res, OETXN_ROLLEDBACK);
    } else {
        UptRes_set_status(res, OETXN_ACTIVE);
    }
    context->res_msgs = Oec_AList_list(arena, res_msg, NULL);
    _default_write_to_client(context);

    return;
}

static void (_supt_update)( T _this_, OEK_Context *context) {

    OekMsg msg       = context->cmd_msg;
    oe_id txnid      = OekMsg_get_tid(msg);
    if ( txnid == 0 ) {
        _send_error(context, 116, OEK_STR_NULL_TXNID);
        return;
    }

    UptCmd cmd       = OekMsg_get_upt_cmd(msg);
    char status_char = UptCmd_get_status(cmd);

    if (!status_char) {
        return _supt_query(_this_, context);
    } else {
        return _supt_update(_this_, context);
    }
}
*/

static void (_supt)( T _this_, OEK_Context *context ) {

    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    OeStore store = client->store;

    Arena_T arena = context->arena;
    OekMsg msg = context->cmd_msg;
    oe_id corid = OekMsg_get_cid(msg);
    oe_id txnid = OekMsg_get_tid(msg);
    UptCmd cmd = OekMsg_get_upt_cmd(msg);
    OESTORE_TXN_STATUS status = _get_txn_status(UptCmd_get_status(cmd));
    oe_dur duration = UptCmd_get_dur(cmd);
    int enlisted = UptCmd_get_enlisted(cmd);

    if ( txnid == 0 ) {
        _send_error(context, 116, OEK_STR_NULL_TXNID);
        return;
    }

    int ret;
    for ( int i = 0 ; i < OEK_MAX_DB_DEADLOCKS ; i++ ) {
        //todo: add fld to store to fail if mismatch
        ret = OeStore_update_txn(store, txnid, duration, status,
                                 arena, enlisted);
        if ( ret != OE_STORE_PLEASE_RETRY ) break;
        OE_DLOG(NULL,OEK_STR_UPT_DL_RETRY, i + 1, txnid);
    }

    if ( ret == OE_STORE_ITEM_NOTFOUND ) {
        _send_error(context, 106, OEK_STR_TXN_NOT_FOUND);
        return;

    } else if ( ret == OESTORE_ENLISTED_COUNT_INCORRECT ) {
        _send_error(context, 107, OEK_STR_WRONG_ENLISTED_COUNT_MSG);
        return;

    } else if ( ret ) {

        OE_ERR( NULL, "%s: %s", OEK_STR_UPT_FAILED, OeStore_strerror(store, ret) );

        char *msg;

        if (ret == OESTORE_UPDATING_COMMITTED_TXN_ERR) {
            msg = OEK_STR_TXN_ALREADY_COMMITTED;
        } else if (ret == OESTORE_UPDATING_ROLLEDBACK_TXN_ERR) {
            msg = OEK_STR_TXN_ALREADY_ROLLEDBACK;
        } else if (ret == OESTORE_UPDATING_EXPIRED_TXN_ERR) {
            msg = OEK_STR_TXN_EXPIRED;
        } else {
            msg = OEK_STR_TXN_UPDATE_FAILED;
            OE_DLOG(NULL,OEK_STR_TXN_UPDATE_ERR, msg, txnid, OeStore_strerror(store, ret));
        }
        _send_error(context, 107, msg);
        return;
    }
    OekMsg res_msg = OekMsg_new(UPT_RES, arena);
    UptRes res = OekMsg_get_upt_res(res_msg);
    OekMsg_set_cid(res_msg, corid);
    OekMsg_set_tid(res_msg, txnid);

    //have to guess for the response what the store thinks the exptime is
    oe_time expire_time;
    if (duration == OEC_FOREVER) {
        expire_time = OEC_FOREVER;
    } else {
        expire_time = oec_get_time_milliseconds() + duration;
    }
    //UptRes_set_expiretime(res, expire_time);

    if ( status == OESTORE_TXN_COMMITTED ) {
        UptRes_set_status(res, OETXN_COMMITTED);
    } else if ( status == OESTORE_TXN_ROLLED_BACK  ) {
        UptRes_set_status(res, OETXN_ROLLEDBACK);
    } else {
        UptRes_set_status(res, OETXN_ACTIVE);
    }
    context->res_msgs = Oec_AList_list(arena, res_msg, NULL);
    _default_write_to_client(context);

    return;
}

static void _upt_async( void *contextp ) {
    OEK_Context *context = (OEK_Context *) contextp;
    assert(context->kernel);
    T _this_ = context->kernel;
    _supt( _this_, context );
}

void (_oek_impl_upt)( T _this_, OEK_Context *context ) {

    context->kernel = _this_;
    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    Oe_Thread_Dispatcher_exec( client->db_dispatcher, _upt_async, context );
}

#undef T

