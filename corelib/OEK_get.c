/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include <stdio.h>
#include "oe_common.h"
#include "Oec_AList.h"
#include "DataObjectList.h"
#include "OeStore.h"
#include "OeStoreQuery.h"
#include "oescript_core_resources.h"
#include "OEK_impl.h"

#define T OEK

static void _async_geth( void * );

static void _no_results(void *contextp) {

    OE_TLOG( NULL, "OEK_get._no_results\n"); //i18n

    OEK_Context *context = (OEK_Context *) contextp;
    Arena_T arena = context->arena;

    OekMsg res_msg = OekMsg_new(GET_RES, arena);
    GetRes res = OekMsg_get_get_res(res_msg);
    OekMsg_set_cid(res_msg, OekMsg_get_cid(context->cmd_msg));
    OekMsg_set_tid(res_msg, OekMsg_get_tid(context->cmd_msg));

    //set destroy flag
    GetCmd cmd = OekMsg_get_get_cmd(context->cmd_msg);
    bool destroy = GetCmd_remove(cmd);
    if (destroy) {
        GetRes_set_destroyed(res);
    }

    Oec_AList res_msgs = Oec_AList_list(arena, res_msg, NULL);

    context->res_msgs = res_msgs;

    _default_write_to_client(context);
    return;
}

static void _geth_handle_result(OEK_Context *context, OeStoreQuery query) {

    OE_TLOG( NULL, "OEK_get._geth_handle_result\n"); //i18n

    assert(context);
    Arena_T arena = context->arena;
    assert(arena);

    OekMsg msg = context->cmd_msg;
    oe_id corid = OekMsg_get_cid(msg);
    oe_id txnid = OekMsg_get_tid(msg);
    GetCmd cmd = OekMsg_get_get_cmd(msg);
    bool destroy = GetCmd_remove(cmd);
    bool istuple = GetCmd_is_tuple(cmd);
    bool retAttrs = GetCmd_return_attrs(cmd);
    bool retBytes = GetCmd_return_bytes(cmd);

    size_t result_count = OeStoreQuery_get_nitems_found(query);
    assert(result_count);
    Oec_AList res_msgs = Oec_AList_list(arena, NULL);
    for (int i = 0; i < result_count; i++) {

        OeStoreHolder result = OeStoreQuery_get_store_holder(query, i);

        OekMsg res_msg = OekMsg_new(GET_RES, arena);
        GetRes res = OekMsg_get_get_res(res_msg);
        OekMsg_set_cid(res_msg, corid);
        OekMsg_set_tid(res_msg, txnid);

        GetRes_set_data(res, OeStoreHolder_get_data(result));
        GetRes_set_nresults(res, result_count);
        if (istuple) {
            GetRes_set_tuple(res);
        }
        if (destroy) {
            GetRes_set_destroyed(res);
        }
        if (retAttrs) {
            GetRes_set_return_attrs(res);
        }
        if (retBytes) {
            GetRes_set_return_bytes(res);
        }

        res_msgs = Oec_AList_append(res_msgs, Oec_AList_list(arena, res_msg, NULL));
    }

    context->res_msgs = res_msgs;
    _default_write_to_client(context);
    return;
}

static void _geth_handle_no_results(OEK_Context *context, OeStoreQuery query) {

    OE_TLOG( NULL, "OEK_get._geth_handle_no_results\n"); //i18n

    assert(context);
    Arena_T arena = context->arena;
    assert(arena);
    OEK _this_ = context->kernel;
    assert(_this_);
    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);

    OekMsg msg = context->cmd_msg;
    GetCmd cmd = OekMsg_get_get_cmd(msg);
    bool destroy = GetCmd_remove(cmd);
    DataObjectList keylists = GetCmd_get_data_objects(cmd);

    if (context->expire_time <= oec_get_time_milliseconds()) {
        _no_results(context);
    } else if (OeStoreQuery_is_ifexists(query) && !OeStoreQuery_exists(query)) {
        //only wait for timeout if there is something promissing to wait for
        _no_results(context);
    } else {
        oe_time dur = context->expire_time - oec_get_time_milliseconds();
        //oe_id saveme =
        Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
        //note watch this for performance issues, the matcher is not
        // interested in tuples with tuple-length and position info,
        // you might be getting false positives leading to retries
        Oed_Dispatcher_reg_group(client->template_matcher,
                                 keylists, dur, destroy, _async_geth,
                                 _async_geth, context);
    }
    return;
}

static void _geth(void *contextp) {

    OE_TLOG( NULL, "OEK_get._geth\n"); //i18n

    OEK_Context *context = (OEK_Context *) contextp;
    assert(context);
    Arena_T arena = context->arena;
    assert(arena);
    OEK _this_ = context->kernel;
    assert(_this_);
    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    OeStore store = client->store;
    assert(store);

    OekMsg msg = context->cmd_msg;
    oe_id corid = OekMsg_get_cid(msg);
    oe_id txnid = OekMsg_get_tid(msg);
    GetCmd cmd = OekMsg_get_get_cmd(msg);
    bool destroy = GetCmd_remove(cmd);
    bool istuple = GetCmd_is_tuple(cmd);
    size_t max_responses = GetCmd_get_max_responses(cmd);
    DataObjectList keylists = GetCmd_get_data_objects(cmd);

    OE_TLOG( NULL, "OEK_get._geth max responses: %i\n", max_responses); //i18n
    OeStoreQuery query = OeStoreQuery_new(context->arena, max_responses);
    OeStoreQuery_set_value_lists(query, keylists);
    OeStoreQuery_set_take(query, destroy);
    OeStoreQuery_set_ifexists(query, GetCmd_ifexists(cmd));
    OE_TLOG( NULL, "OEK_get._geth is tuple: %i\n", istuple); //i18n
    OeStoreQuery_set_tuple(query, istuple);
    OeStoreQuery_set_owner(query, txnid);

    unsigned int ret;
    for ( int i = 0 ; i < OEK_MAX_DB_DEADLOCKS ; i++ ) {
        ret = OeStore_get(store, query);
        if ( ret != OE_STORE_PLEASE_RETRY ) break;
        OeStoreQuery_reset_nitems_found(query);
        OE_DLOG( NULL, OEK_STR_GET_DL_RETRY, i + 1,
                 OekMsg_get_tid(msg), destroy ? "take" : "read",
                 i + 1 == OEK_MAX_DB_DEADLOCKS ? " max deadlocks" : "" );
    }

    if ( ret && ret != OE_STORE_ITEM_NOTFOUND ) {

        OE_ERR(NULL, "%s: %s", OEK_STR_GET_FAILED, OeStore_strerror(store, ret));

        _send_error(context, 105, OEK_STR_GET_FAILED);

        return;

    } else if (ret == OE_STORE_ITEM_NOTFOUND || OeStoreQuery_get_nitems_found(query) == 0) {

        _geth_handle_no_results(context, query);

    } else {

        _geth_handle_result(context, query);

    }

    return;
}

static void _async_geth( void *contextp ) {

    OEK_Context *context = (OEK_Context *) contextp;
    T _this_ = context->kernel;
    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    Oe_Thread_Dispatcher_exec(client->db_dispatcher, _geth, context);
}

void (_oek_impl_get)( T _this_, OEK_Context *context ) {

    OE_TLOG( NULL, "OEK_get._oek_impl_get\n"); //i18n
    //context->kernel = _this_;
    assert(context->kernel);
    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    Oe_Thread_Dispatcher_exec(client->db_dispatcher, _geth, context);
}

#undef T

