/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "oe_common.h"
#include "Oec_IdSet.h"
#include "OEK.h"
#include "OekMsg.h"
#include "OeNet.h"
#include "OepClient.h"
#include "Oed_Dispatcher.h"
#include "oep_resources.h"
#include "oep_utils.h"
#include "Oec_Future.h"
#include "OeNetDefaultFactory.h"
#include "Subject.h"

#define T OepClient

typedef struct ACContext {

    OEK_Context *context;
    oe_id        sid;
    T            proto;

} ACContext;

typedef struct CID_KEY {
    oe_id cid;
} CID_KEY;

static int cidcmp(const void *x, const void *y) {
    CID_KEY *xitem = (CID_KEY *) x;
    CID_KEY *yitem = (CID_KEY *) y;
    oe_id xint = (oe_id) xitem->cid;
    oe_id yint = (oe_id) yitem->cid;
    if (xint == yint) return 0;
    if (xint < yint) return -1;
    return 1;
}
static unsigned cidhash(const void *x) {
    CID_KEY *xitem = (CID_KEY *) x;
    oe_id xint = (oe_id) xitem->cid;
    return(unsigned) xint;
}

static ACContext *_restore_context(T _this_, oe_id cid) {
    OE_DLOG(0, OEP_CLIENT_RESTORE_C_W_CID, cid);
    ACContext *pc;
    CID_KEY key;
    key.cid = cid;
    pc = (ACContext *) Table_remove(_this_->pending, &key);
    if (!pc) OE_ERR(NULL, OEP_NO_CID, cid);
    assert(pc);
    return pc;
}

//todo: implement some throttle related to outstanding responses
static void _save_context(T _this_, oe_id cid, ACContext *pc) {
    OE_DLOG(0, OEP_CLIENT_SAVE_C_W_CID, cid);
    assert(_this_);
    assert(_this_->pending);
    assert(cid);
    assert(pc);
    CID_KEY *key;
    key = Arena_alloc(pc->context->arena, sizeof *key, __FILE__, __LINE__);
    key->cid = cid;
    Table_put(_this_->pending, key, pc);
}

static void _send_con_closed(OEK_Context *context) {
    OekMsg err_msg = OekMsg_new(OEK_ERR, context->arena);
    OekErr err = OekMsg_get_err(err_msg);
    OekErr_set_type(err, 1);
    OekErr_set_message(err, (oe_scalar) OEP_STR_CON_CLOSED);
    context->err_msg = err_msg;
    context->write_response_cb(context);
}

static void _cleanup(OeSession session) {

    T _this_ = OeNet_get_proto(OeSession_get_net(session));
    int sz = Table_length(_this_->pending);
    void **array = Table_toArray(_this_->pending, NULL);
    for ( int i = 0; i < sz && array[i]; i+= 2 ) {
        CID_KEY *key = (CID_KEY *) array[i];
        ACContext *pc = Table_remove(_this_->pending, key);
        _send_con_closed(pc->context);
    }
    Mem_free(array, __FILE__, __LINE__);
    Table_free(&_this_->pending);
    //OepClient_free(&_this_);  //closing the proto is hard and buggy.
}

static void _append_res_msg(OEK_Context *context, OekMsg msg) {
    if (!context->res_msgs) {
        context->res_msgs = Oec_AList_list(context->arena, msg, NULL);
    } else {
        context->res_msgs = Oec_AList_append(context->res_msgs, Oec_AList_list(context->arena, msg, NULL));
    }
}

static void _buffered_on_read(OeSession session) {

    OE_TLOG(NULL, "OepClient._buffered_on_read\n");

    assert(session);
    if (OeSession_state_is_set(session, OENET_CLIENT_CLOSING)) {
        _cleanup(session);
        return;
    }
    OEK_Context *context = NULL;

    T _this_ = OeNet_get_proto(OeSession_get_net(session));
    assert(_this_);

    oe_id cid;
    int rc = _this_->peek_cid_fun(session, &cid);
    if (rc < 0) goto error;
    if (!rc) goto quit;

    ACContext *pc = _restore_context(_this_, cid);
    assert(pc);
    context = pc->context;
    assert(context);
    OE_DLOG(0, OEP_CLIENT_RESTORE_C_SUCCESS, cid);

    OekMsg msg = NULL;
    rc = _this_->read_fun(session, &msg, context->arena);
    if (rc < 0) goto error;
    if (!rc) {
        _save_context(_this_, cid, pc); //not yet
        goto quit;
    }

    OEK_MSG_TYPE mtype = OekMsg_get_type(msg);
    OE_DLOG(0, OEP_CLIENT_READ_TYPE, mtype);
    if (context->write_response_cb) {
        if (mtype == OEK_ERR) {
            context->err_msg = msg;
            context->write_response_cb(context);
        } else if (mtype == GET_RES) {
            _append_res_msg(context, msg);
            GetRes res = OekMsg_get_get_res(msg);
            if (GetRes_get_nresults(res) > Oec_AList_length(context->res_msgs)) {
                _save_context(_this_, cid, pc); //wait until you have all the responses
            } else {
                context->write_response_cb(context);
            }
        } else {
            _append_res_msg(context, msg);
            context->write_response_cb(context);
        }
    } else {
        OE_ERR(0, OEP_CLIENT_CB_NOT_SET, mtype);
    }

    //recurse thru batch of commands
    if (OeNetConn_available(OeSession_get_conn(session)) > 0 ) {
        _buffered_on_read(session);
    }

    quit:
    return;

    error:
    OE_ERR(-1, OEP_CLIENT_PARSE_ERROR);
    if (context) {
        OekMsg errmsg = OekMsg_new(OEK_ERR, context->arena);
        OekErr err = OekMsg_get_err(errmsg);
        OekErr_set_message(err, (oe_scalar) OEP_CLIENT_PARSE_ERROR);
        context->err_msg = errmsg;
        context->write_response_cb(context);
    }
}

static ACContext *_create_oeproto_ac_context(T _this_, OEK_Context *context) {
    assert(_this_);
    assert(context);
    ACContext *pc;
    pc = Arena_calloc(context->arena, 1, sizeof(ACContext), __FILE__, __LINE__);
    pc->context = context;
    pc->proto = _this_;
    pc->sid = OeSession_get_id(_this_->session);
    return pc;
}

static void _write_to_server_cb(void *pcp){
    /* //todo: check!!!!!!!!!!!!
    if (OeSession_state_is_set(session, OENET_CLIENT_CLOSING)) {
        _cleanup(session);
        return;
    }
    */

    OE_DLOG(NULL, "OepClient._write_to_server_cb\n");
    ACContext *pc = (ACContext *) pcp;
    assert(pc);
    T _this_ = pc->proto;
    assert(_this_);
    OEK_Context *context = pc->context;
    assert(context);
    assert(context->arena);

    oe_id cid = OekMsg_get_cid(context->cmd_msg);
    _save_context(_this_, cid, pc);

    Oec_IdSet idset = OeNet_get_sessions(_this_->net);
    if (Oec_IdSet_member(idset, pc->sid)) {
        OeSession_decr_task_count(_this_->session);
        OE_DLOG(NULL, "OepClient._write_to_server_cb write_fun\n");
        _this_->write_fun(_this_->session, context->cmd_msg, context->arena);
    } else {
        OE_ERR(NULL, OEP_NO_CONN, cid);
        _send_con_closed(context);
    }
}

static void _con_cb(OEK_Context *context) {

    Oec_Future f = (Oec_Future) context->p_context;
    Oec_Future_set(f, context);
}

T OepClient_new(Oed_Dispatcher dispatcher,
                OeNet net, oe_scalar username,
                oe_scalar pwd, char *spec) {

    assert(dispatcher);
    Arena_T arena = Arena_new();
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    _this_->dispatcher = dispatcher;
    _this_->pending = Table_new(100, cidcmp, cidhash);
    _this_->spec = spec;
    Subject s = Subject_new(arena);
    _this_->subject = s;
    Subject_set_username(s, username);
    Subject_set_pwd(s, pwd);

    OeNet_set_read_handler(net, _buffered_on_read);
    _this_->net = net;
    OeNet_set_proto(net, _this_);
    assert(_this_->net);

    return _this_;
}
void OepClient_stop(T _this_) {
    OeNet_stop(_this_->net);
}
void OepClient_stats(T _this_) {
    printf("ejs OepClient stats\n");
}
void OepClient_free(T *acp) {
    assert(acp && *acp);
    T _this_ = *acp;
    assert(_this_);
    assert(_this_->arena);
    assert(_this_->net);
    OeSession session = _this_->session;
    //OeNet_stop(_this_->net);
    //OeNet_free(&_this_->net);
    Arena_dispose(&_this_->arena);
    if (session) {
        OeSession_free(&session);
    }
    *acp = NULL;
}

void OepClient_kfree(OEK *kp) {
    T _this_ = OEK_get_client(*kp);
    assert(_this_);
    if (_this_->session) {
        //ejs FIXME
        //OeNet_cleanup_conn(_this_->session);   //segfault in lua
                                                 // test_net_jcl_multiple_extents.lua
        _this_->session = NULL;
    }
    //OepClient_free(&_this_);
    //not the orderly close of the conn needs the protocol alive.
    //free has been moved to _cleanup
}

int OepClient_connect(T _this_) {

    assert(_this_->peek_cid_fun);
    assert(_this_->read_fun);
    assert(_this_->write_fun);

    int rc = 0;
    OEK_Context *context = OEK_Context_new();
    context->kernel = _this_;
    context->write_response_cb = _con_cb;

    Subject subject = _this_->subject;
    char * spec = _this_->spec;
    char *hostname = oepu_get_hostname_from_spec(_this_->arena,spec);
    if (!hostname) return -1;
    oe_scalar extentname = oepu_get_extentname_from_spec(_this_->arena,spec);
    int port = oepu_get_port_from_spec(spec);
    if (port <= 0) return -1;
    void *conctx = OeNet_connect(_this_->net, hostname, port);
    assert(conctx);

    OekMsg cmdmsg = OekMsg_new(CON_CMD, context->arena);
    OekMsg_set_cid(cmdmsg, 1);
    ConCmd cmd = OekMsg_get_con_cmd(cmdmsg);
    ConCmd_set_extentname(cmd, extentname ? extentname : (oe_scalar) "default"); //todo: lookup
    ConCmd_set_username(cmd, Subject_get_username(subject));
    ConCmd_set_pwd(cmd, Subject_get_pwd(subject));
    ConCmd_set_version_maj(cmd, OE_VERSION_MAJ);
    ConCmd_set_version_min(cmd, OE_VERSION_MIN);
    ConCmd_set_version_upt(cmd, OE_VERSION_UPT);
    ConCmd_set_rollback_on_disconnect(cmd);

    OeSession session = OeNet_listen_for_bytes(_this_->net, conctx);
    OeSession_set_id(session, 1); //temp id
    _this_->session = session;

    context->cmd_msg = cmdmsg;
    ACContext *pc = _create_oeproto_ac_context(_this_, context);

    //set a callback with a future and log the sid and return meaningful rc
    Oec_Future f = Oec_Future_new();
    context->p_context = f;

    Oed_Dispatcher_exec(_this_->dispatcher, _write_to_server_cb, pc);

    OEK_Context *c = Oec_Future_get(f, OEC_FOREVER);
    assert(c);
    OekMsg resmsg;
    if (c->err_msg) {
        //resmsg = Oec_AList_get(c->err_msg, 0);
        resmsg = c->err_msg;
    } else {
        resmsg = Oec_AList_get(c->res_msgs, 0);
    }
    assert(resmsg);
    OEK_MSG_TYPE t = OekMsg_get_type(resmsg);
    switch (t) {
    case CON_RES:
        OE_DLOG(0, OEP_CLIENT_CONNECTED, ConRes_get_sid(OekMsg_get_con_res(resmsg)));
        rc = 0;
        OeSession_state_set( session, OENET_CLIENT_CONNECTED );
        break;
    case OEK_ERR:
        OE_ERR(-1, OEP_CLIENT_CONNECT_ERR, 
               (char *) OekErr_get_message(OekMsg_get_err(resmsg)));
        rc = -1;
        break;
    default:
        assert(false);
        rc = -1;
        break;
    }
    OEK_Context_free(&context);
    Oec_Future_free(&f);
    return rc;
}

void OepClient_send(OEK kernel, OEK_Context *context) {
    OE_DLOG(NULL, "OepClient._send\n");

    T _this_ = OEK_get_client(kernel);

    assert(_this_);
    assert(context);

    ACContext *pc = _create_oeproto_ac_context(_this_, context);

    assert(_this_->dispatcher);
    Oed_Dispatcher_exec(_this_->dispatcher, _write_to_server_cb, pc);
}

//make a kernel api wrapper around the async protocol client
OEK OepClient_new_kernel(T _this_) {
    assert(_this_);
    assert(_this_->peek_cid_fun);
    assert(_this_->read_fun);
    assert(_this_->write_fun);

    OEK kernel = OEK_new(_this_,
                         OepClient_send,
                         OepClient_send,
                         OepClient_send,
                         OepClient_send,
                         OepClient_send,
                         OepClient_kfree,
                         OepClient_stop,
                         OepClient_stats);
    return kernel;
}

void OepClient_configure(T _this_,
                         oep_client_peek_msg  peek_cid_fun,
                         oep_client_read_msg  create_msg_fun,
                         oep_client_write_msg out_fun) {

    _this_->peek_cid_fun = peek_cid_fun;
    _this_->read_fun = create_msg_fun;
    _this_->write_fun    = out_fun;
}

#undef T

