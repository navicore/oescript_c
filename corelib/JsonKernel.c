/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "JsonKernel.h"
#include "OEK.h"
#include "Oec_AList.h"
#include "json_msg_factory.h"
#include "arena.h"
#include "mem.h"
#include "oec_log.h"

#define T JsonKernel

static OEK_Context *_new_context(void);
static void _write_to_client(OEK_Context *context);

struct json_cb {
    json_callback *cb;
    void *cb_arg;
};

T JsonKernel_new(OEK kernel) {
    assert(kernel);
    T _this_;
    _this_ = Mem_calloc( 1, sizeof *_this_, __FILE__, __LINE__ );
    _this_->kernel = kernel;
    return _this_;
}

int JsonKernel_exec(T _this_, char *json_cmd, json_callback *cb, void *cb_arg) {
    assert(_this_);
    assert(json_cmd);
    assert(cb);

    OEK_Context *context = _new_context();
    context->kernel = _this_->kernel;
    Arena_T arena = context->arena;
    struct json_cb *cbholder = Arena_alloc(arena, sizeof(struct json_cb) , __FILE__, __LINE__ );
    cbholder->cb = cb;
    cbholder->cb_arg = cb_arg;
    context->p_context = cbholder;
    OekMsg msg = NULL;
    int rc = json_msg_create(&msg, json_cmd, arena);

    if (!msg) {
        OE_DLOG(0, "JsonKernel_exec rc %i, json: %s\n", rc, json_cmd);
        Arena_free(arena);
        return -1;
    }
    context->start_time = oec_get_time_milliseconds();

    //set context lifespan
    OEK_MSG_TYPE mtype = OekMsg_get_type(msg);
    GetCmd cmd;
    switch (mtype) {
    case GET_CMD:
        cmd = OekMsg_get_get_cmd(msg);
        context->expire_time = context->start_time + GetCmd_get_timeout(cmd);
        break;
    case CON_CMD:
        break;
    case DIS_CMD:
        break;
    case PUT_CMD:
        break;
    case STA_CMD:
        break;
    case UPT_CMD:
        break;
    case UPL_CMD:
        break;
    case OEK_ERR:
        break;
    case CON_RES:
        break;
    case PUT_RES:
        break;
    case GET_RES:
        break;
    case STA_RES:
        break;
    case UPT_RES:
        break;
    case UPL_RES:
        break;
    case OEK_NON:
    default:
        assert(false);
        Arena_free(arena);
        return -1;
    }
    context->cmd_msg = msg;
    OEK_exec(_this_->kernel, context);
    return 0;
}

void JsonKernel_free(T *objp) {
    assert(objp && *objp);
    T _this_ = *objp;
    Mem_free( _this_, __FILE__, __LINE__ );
}

static OEK_Context *_new_context(void) {

    Arena_T arena = Arena_new();
    OEK_Context *context;
    context = Arena_calloc(arena, 1, sizeof *context, __FILE__, __LINE__);
    context->arena = arena;
    context->write_response_cb = _write_to_client;

    return context;
}

static void _write_to_client(OEK_Context *context) {

    Arena_T arena = context->arena;

    char *jbuff = NULL;
    if (context->err_msg) {
        int rc = json_msg_serialize(context->err_msg, &jbuff, arena);
        assert(!rc);
    } else {
        ArrayIterator iter = Oec_AList_iterator(context->res_msgs);
        while (ArrayIterator_hasMore(iter)) {
            OekMsg res = (OekMsg) ArrayIterator_next(iter);
            char *ibuff;
            int rc = json_msg_serialize(res, &ibuff, arena);
            assert(!rc);
            if (!jbuff) {
                jbuff = ibuff;
            } else {
                jbuff = oec_str_concat(arena, 3, jbuff, "\n", ibuff);
            }
        }
    }
    struct json_cb *cbholder =  (struct json_cb *) context->p_context;
    cbholder->cb(jbuff, cbholder->cb_arg);
    OE_TLOG(0, "JsonKernel _write_to_client:\n%s\n", jbuff);

    Arena_free(arena);
}

#undef T

