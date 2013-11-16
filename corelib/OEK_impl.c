/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "oe_common.h"
#include "oec_threads.h"
#include "Oed_Dispatcher.h"
#include "Oe_Thread_Dispatcher.h"
#include "OeStore.h"
#include "oescript_core_resources.h"
#include "OEK_impl.h"
#include "OEK_default_factory.h"
#include "OEK.h"
#include "oec_log.h"

#define T OEK

#define ST_TYPE static
//#define ST_TYPE

ST_TYPE void _lease_reaper_cb( void *arg ) {
    Oek_Impl_ *holder = (Oek_Impl_ *) arg;
    assert(holder);
    OeStore store = holder->store;
    assert(store);

    while ( OeStore_lease_reaper(store) > 0 ) {
        usleep(1000 * 50);
    }
    Oed_Dispatcher_timed_exec( holder->template_matcher, _lease_reaper_cb, holder, 30000 ); //todo: config
}

ST_TYPE void _fire_write_event( DataObject props, void *matcherp ) {

    OE_TLOG( NULL, "OEK_impl _fire_write_event\n" ); //i18n
    Oed_Dispatcher matcher = (Oed_Dispatcher) matcherp;

    Oed_Dispatcher_process(matcher, props, true);
}

Oed_Dispatcher _get_matcher(T _this_) {
    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    return client->template_matcher;
}
Oe_Thread_Dispatcher _get_db_dispatcher(T _this_) {
    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    return client->db_dispatcher;
}
OeStore _get_store(T _this_) {
    Oek_Impl_ *client = (Oek_Impl_*) OEK_get_client(_this_);
    return client->store;
}

ST_TYPE void _oekstats(T _this_) {

    assert(_this_);
    printf("ejs oek stats\n");
}

ST_TYPE void _oekfree(T *_this_p) {

    assert(_this_p && *_this_p);
    T _this_ = *_this_p;
    Mem_free(_this_, __FILE__, __LINE__);
}

oe_id (OEK_create_id)( T _this_ ) {

    OeStore store = _get_store(_this_);
    assert(store);

    return OeStore_create_id( store );
}

OEK_Context *(OEK_Context_new)(void) {

    Arena_T arena = Arena_new();
    OEK_Context *context;
    context = Arena_calloc(arena, 1, sizeof *context, __FILE__, __LINE__);
    context->arena = arena;

    return context;
}

void (OEK_Context_free)( OEK_Context **contextp ) {
    assert(contextp && *contextp);
    OEK_Context *context = *contextp;
    Arena_T arena = context->arena;
    Arena_dispose( &arena );
}

void _send_error( OEK_Context *context, oe_id errid, char *errstring ) {

    assert(context);
    OekMsg cmd_msg = context->cmd_msg;
    OekMsg res_msg = OekMsg_new(OEK_ERR, context->arena);
    OekErr err = OekMsg_get_err(res_msg);
    if (cmd_msg) {
        OekMsg_set_cid(res_msg, OekMsg_get_cid(cmd_msg));
        OekMsg_set_tid(res_msg, OekMsg_get_tid(cmd_msg));
    }
    OekErr_set_type(err, errid);
    OekErr_set_message(err, (oe_scalar) errstring);
    context->err_msg = res_msg;
    _default_write_to_client(context);
}

T OEK_default_factory_new(Oed_Dispatcher template_matcher,
               Oe_Thread_Dispatcher db_dispatcher, OeStore store) {

    Oek_Impl_ *holder;
    holder = Mem_calloc( 1, sizeof *holder, __FILE__, __LINE__ );
    assert(store);
    holder->store = store;
    holder->template_matcher = template_matcher;
    holder->db_dispatcher = db_dispatcher;
    Oed_Dispatcher_timed_exec( template_matcher, _lease_reaper_cb, holder, 30000 );

    T _this_ = OEK_new(holder, _oek_impl_put, _oek_impl_get, _oek_impl_sta, _oek_impl_upt, _oek_impl_upl, _oekfree, NULL, _oekstats);

    OeStore_set_fire_write_event_cb( store, _fire_write_event, template_matcher );

    OE_DLOG( NULL, OEK_STR_SETUP );

    return _this_;
}

void _default_write_to_client(OEK_Context *context) {
    if (context->write_response_cb) {
        OE_DLOG( NULL, "_default_write_to_client" );
        context->write_response_cb(context);
    } else {
        //noop.  probably an automatic rollback from a disconect
        oe_id tid, cid;
        if (context->err_msg) {
            cid = OekMsg_get_cid(context->err_msg);
            tid = OekMsg_get_tid(context->err_msg);
        } else if (context->res_msgs) {
            OekMsg msg = Oec_AList_get(context->res_msgs, 0);
            if (msg) {
                cid = OekMsg_get_cid(msg);
                tid = OekMsg_get_tid(msg);
            } else {
                cid = 0;
                tid = 0;
            }
        } else { 
            cid = 0;
            tid = 0;
        }
        OE_DLOG(NULL, OEK_STR_NO_CALLBACK, tid, cid);
        OEK_Context_free(&context);
    }
}

#undef T

