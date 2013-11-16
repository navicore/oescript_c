/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

#include "OepServer.h"
#include "OEK.h"
#include "oe_common.h"
#include "OekMsg.h"
#include "OeNet.h"
#include "OeNetConn.h"
#include "OeSession.h"
#include "OepClient.h"
#include "oep_resources.h"
#include "SecurityContext.h"
#include "LoginModule.h"
#include "Subject.h"
#include "table.h"
#include "SKeyTable.h"

#define T OepServer

/**
 * you make a OepServer per protocol, and each server can share 
 * dispatchers and event_base(s).  each server can start any 
 * number of ports for that 1 protocol they are configured for. 
 */

//for info needed for the life of the transaction
typedef struct OepDocContext {

    OeSession      session;

} OepDocContext;

OEK _get_kernel(T _this_, oe_scalar extentname) {
    OEK k = (OEK) Table_get(_this_->kernels, (void *)extentname);
    return k;
}

static void _con_timeout_cb(OeSession session) {

    if ( !session ) return;

    OeNetConn_clear_timeout(OeSession_get_conn(session));

    if ( OeSession_state_is_set( session, OENET_CLIENT_CONNECTED ) ) {
        return; //is connected.  no prob.
    }

    T _this_ = (T) OeNet_get_proto(OeSession_get_net(session));
    Arena_T arena = OeSession_get_arena(session);
    OekMsg msg = OekMsg_new(OEK_ERR, arena);
    OekErr err = OekMsg_get_err(msg);
    OekErr_set_message(err, (oe_scalar) OEP_SERVER_CONN_TIMEOUT);
    _this_->write_fun(session, msg, arena);

    OE_DLOG( NULL, OEP_STR_CON_TIMEOUT);
    OeNet_cleanup_conn(session);
}

void _free_context(OEK_Context *context) {
    if (context) OEK_Context_free(&context);
}

static int _send_err(OEK_Context *context, char *emsg, oe_id cid) {
    assert(context);
    OekMsg errmsg = OekMsg_new(OEK_ERR, context->arena);
    context->err_msg = errmsg;
    OekMsg_set_cid(errmsg, cid);
    OekErr err = OekMsg_get_err(errmsg);
    OekErr_set_type(err, 1);
    OekErr_set_message(err, (oe_scalar) emsg);
    OepDocContext *p_context = context->p_context;
    int rc = 0;
    if (p_context) {
        OeSession session = p_context->session;
        T _this_ = (T) OeNet_get_proto(OeSession_get_net(session));
        assert(_this_);
        _this_->write_fun(session, errmsg, context->arena);
    }
    _free_context(context);
    return rc;
}

static int _con_handler(OekMsg msg, OeSession session, OEK_Context *context) {

    assert(session);
    T _this_ = (T) OeNet_get_proto(OeSession_get_net(session));
    assert(_this_);
    assert(context);
    OepDocContext *p_context = (OepDocContext *) context->p_context;
    assert(p_context);

    if ( OeSession_state_is_set( session, OENET_CLIENT_CONNECTED ) ) {
        _send_err(context, OEP_SERVER_ALREADY_CONNECTED, OekMsg_get_cid(msg));
        return 0;

    } else {
        ConCmd cmd = OekMsg_get_con_cmd(msg);

        oe_scalar extentname = ConCmd_get_extentname(cmd);
        if (!extentname) {
            OE_DLOG( NULL, "no extentname\n"); //i18n
            _send_err(context, OEP_SERVER_ACCESS_DENIED, OekMsg_get_cid(msg));
            return 0;
        }

        //todo: check version
        //check user
        Subject s = Subject_new(OeSession_get_arena(session));
        OeSession_set_subject(session, s);
        Subject_set_username(s, ConCmd_get_username(cmd));
        Subject_set_pwd(s, ConCmd_get_pwd(cmd));
        Subject_set_extentname(s, extentname);
        if (_this_->security_context) {
            SecurityContext_login(_this_->security_context, s);
            if (!Subject_is_authenticated(s)) {
                OE_DLOG( NULL, "login failed\n"); //i18n
                _send_err(context, OEP_SERVER_ACCESS_DENIED, OekMsg_get_cid(msg));
                return 0;
            }
        }

        //lookup kernel in _this_ by extentname
        OEK kernel = _get_kernel(_this_, extentname);
        if (!kernel) {
            OE_DLOG( NULL, "server not configured for extent %s\n", extentname); //i18n
            _send_err(context, OEP_SERVER_ACCESS_DENIED, OekMsg_get_cid(msg));
            return 0;
        }
        OeSession_set_sess_context(session, kernel);

        OeSession_state_set( session, OENET_CLIENT_CONNECTED );
        if (ConCmd_rollback_on_disconnect(cmd)) {
            OeSession_set_rb_on_disconnect( session );
        }

        OekMsg resmsg = OekMsg_new(CON_RES, context->arena);
        OekMsg_set_cid(resmsg, OekMsg_get_cid(msg));
        ConRes res = OekMsg_get_con_res(resmsg);
        ConRes_set_extentname(res,  extentname);
        ConRes_set_sid(res, OeSession_get_id(session));
        ConRes_set_version_maj(res, OE_VERSION_MAJ);
        ConRes_set_version_min(res, OE_VERSION_MIN);
        ConRes_set_version_upt(res, OE_VERSION_UPT);
        context->res_msgs = Oec_AList_list(context->arena, resmsg, NULL);

        _this_->write_fun(session, resmsg, context->arena);

        _free_context(context);
        return 1;
    }
}

static void _dis_handler(OeSession session) {

    OeSession_state_clear( session, !OENET_CLIENT_CONNECTED );
    OE_DLOG( NULL, OEP_DIS_CLOSE );
    OeNet_cleanup_conn(session);
}

static void _rollback_transactions(OeSession session) {

    assert(session);
    if (!OeSession_rb_on_disconnect(session))  return;

    Set_T txns = OeSession_get_tids(session);
    if (!txns) return;

    OEK kernel = (OEK) OeSession_get_sess_context(session);

    void **tids = Set_toArray(txns, NULL);
    for ( int i = 0; tids[i]; i++ ) {
        oe_id tid = (oe_id) tids[i];
        OE_DLOG(NULL, OEP_ROLLBACK_ON_DIS, tid);
        OEK_Context *tmp_context = OEK_Context_new();
        tmp_context->kernel = kernel;
        OekMsg cmdmsg = OekMsg_new(UPT_CMD, tmp_context->arena);
        OekMsg_set_tid(cmdmsg, tid);
        UptCmd cmd = OekMsg_get_upt_cmd(cmdmsg);
        UptCmd_set_status(cmd, OETXN_ROLLEDBACK);
        tmp_context->cmd_msg = cmdmsg;
        OEK_upt(kernel, tmp_context);
    }
    Mem_free(tids, __FILE__, __LINE__);
    Set_free(&txns);
}

static void _update_pending_txns(OEK_Context *context) {
    if (context->res_msgs) {
        OepDocContext *pdc = (OepDocContext *) context->p_context;
        OeSession session = pdc->session;
        OekMsg resmsg = Oec_AList_get(context->res_msgs, 0);
        OEK_MSG_TYPE rtype = OekMsg_get_type(resmsg);
        char status;
        switch (rtype) {
        case STA_RES:
            OeSession_save_tid(session, OekMsg_get_tid(resmsg));
            break;
        case UPT_RES:
            status = UptRes_get_status(OekMsg_get_upt_res(resmsg));
            if (status != OETXN_ACTIVE) {
                OeSession_clear_tid(session, OekMsg_get_tid(resmsg));
            }
            break;
        default:
            break;
        }
    }
}

static void _write_to_client_cb(void *contextp) {

    OE_DLOG(NULL, "OepServer._write_to_client_cb\n");

    OEK_Context *context = (OEK_Context *) contextp;
    _update_pending_txns(context);
    OepDocContext *pdc = (OepDocContext *) context->p_context;

    OeSession session = pdc->session;
    int tasks = OeSession_decr_task_count(session);
    bool client_disconnected = !OeSession_state_is_set(session, OENET_CLIENT_CONNECTED);

    T _this_ = (T) OeNet_get_proto(OeSession_get_net(session));
    assert(_this_);

    if (!tasks && client_disconnected) { //conn is closed and you are the last task, rollback

        _rollback_transactions(session);
        OeNet_cleanup_conn(session);

    } else if (!client_disconnected) { //else if conn is still good

        Arena_T arena = context->arena;
        if (context->err_msg) {
            OE_DLOG(NULL, "OepServer._write_to_client_cb sending error\n");
            _this_->write_fun(session, context->err_msg, arena);
            OeNet_cleanup_conn(session);
        } else {
            Oec_AList res_msgs = context->res_msgs;
            size_t sz = Oec_AList_length(res_msgs);
            OekMsg *res_msg_array = (OekMsg*) Oec_AList_toArray(res_msgs, arena, NULL);
            for (int i = 0; res_msg_array[i] && i < sz; i++) {
                OekMsg res_msg = res_msg_array[i];
                OE_DLOG(NULL, "OepServer._write_to_client_cb sending\n");
                _this_->write_fun(session, res_msg, arena);
            }
        }
    }
    _free_context(context);
}

static void _write_to_client(OEK_Context *context) {

    OE_DLOG(NULL, "OepServer._write_to_client\n");
    OepDocContext *pdc = (OepDocContext *) context->p_context;
    OeSession session = pdc->session;
    T _this_ = (T) OeNet_get_proto(OeSession_get_net(session));
    Oed_Dispatcher_exec(_this_->dispatcher, _write_to_client_cb, context);
}

static int _invoke_msg_handler(OekMsg msg, OeSession session, OEK_Context *context) {

    assert(context);

    OEK_MSG_TYPE mtype = OekMsg_get_type(msg);
    context->write_response_cb = _write_to_client;
    context->cmd_msg = msg;

    T _this_ = (T) OeNet_get_proto(OeSession_get_net(session));
    assert(_this_);

    //ejs todo: need to make this seda stages so that auth isn't blocking IO
    if (_this_->security_context) {
        if (mtype != CON_CMD) {
            Subject s = (Subject) OeSession_get_subject(session);
            if (s == NULL) {
                _send_err(context, OEP_ERR_NOT_LOGGED_IN, OekMsg_get_cid(msg));
                return 0;
            }
            if (!Subject_is_authenticated(s)) {
                _send_err(context, OEP_ERR_NOT_LOGGED_IN, OekMsg_get_cid(msg));
                return 0;
            }
            AUTH auth = SecurityContext_authorize(_this_->security_context, s, msg);
            if (auth != ALLOW) {
                OE_DLOG( NULL, "authorization failed\n"); //i18n
                _send_err(context, OEP_SERVER_ACCESS_DENIED, OekMsg_get_cid(msg));
                return 0;
            }
        }
    }

    OEK kernel = (OEK) OeSession_get_sess_context(session);
    if (mtype != CON_CMD) {
        if (!kernel) {
            _send_err(context, OEP_ERR_NOT_LOGGED_IN, OekMsg_get_cid(msg));
            return 0;
        }
    }
    context->kernel = kernel;

    switch (mtype) {
    case CON_CMD:
        return _con_handler(msg, session, context);
    case DIS_CMD:
        _dis_handler(session);
        _free_context(context);
        return 1;
    case GET_CMD:
        OeSession_incr_task_count(session);
        GetCmd cmd = OekMsg_get_get_cmd(msg);
        context->expire_time = oec_get_time_milliseconds() + GetCmd_get_timeout(cmd);
        OEK_get(kernel, context);
        return 1;
    case PUT_CMD:
        OeSession_incr_task_count(session);
        OEK_put(kernel, context);
        return 1;
    case STA_CMD:
        OeSession_incr_task_count(session);
        OEK_sta(kernel, context);
        return 1;
    case UPT_CMD:
        OeSession_incr_task_count(session);
        OEK_upt(kernel, context);
        return 1;
    case UPL_CMD:
        OeSession_incr_task_count(session);
        OEK_upl(kernel, context);
        return 1;
    case OEK_ERR:
    case CON_RES:
    case PUT_RES:
    case GET_RES:
    case STA_RES:
    case UPT_RES:
    case UPL_RES:
    case OEK_NON:
    default:
        assert(false);
        return 0;
    }
}

static void _cleanup(OeSession session) {

    assert(session);

    int tasks = OeSession_get_task_count(session);

    if (!tasks) { //conn is closed and no tasks are running, rollback
        _rollback_transactions(session);
    }
}

static OepDocContext *_create_pcontext(OeSession session, Arena_T arena) {

    OepDocContext *p_context;
    p_context = Arena_calloc(arena, 1, sizeof *p_context, __FILE__, __LINE__);
    p_context->session = session;
    return p_context;
}

static OEK_Context *_create_context(OeSession session) {
    OEK_Context *context = OEK_Context_new();
    Arena_T arena = context->arena;
    context->start_time = oec_get_time_milliseconds();
    context->p_context = _create_pcontext(session, arena);
    context->cmd_msg = NULL;
    OeSession_set_appl_context(session, context);
    return context;
}

static void _buffered_on_read(OeSession session) {

    assert(session);
    if (OeSession_state_is_set(session, OENET_CLIENT_CLOSING)) {
        _cleanup(session);
        return;
    }

    T _this_ = (T) OeNet_get_proto(OeSession_get_net(session));
    assert(_this_);

    OEK_Context *context = OeSession_get_appl_context(session);
    if (!context) {
        context = _create_context(session);
    }

    OekMsg msg = context->cmd_msg;
    int rc = _this_->read_fun(session, &msg, context->arena);
    if (rc < 0) {
        _send_err(context, OEP_ERR_CAN_NOT_PARSE, 0);
        goto error; //fatal
    }
    if (!rc) {
        //nothing yet
        context->cmd_msg = msg; //but maybe a partial
        goto quit;
    }
    OeSession_set_appl_context(session, NULL);

    OEK_MSG_TYPE mtype = OekMsg_get_type(msg);

    if (!OeSession_state_is_set(session, OENET_CLIENT_CONNECTED ) && mtype != CON_CMD) {
        _send_err(context, OEP_ERR_NOT_CONNECTED, OekMsg_get_cid(msg));
        goto error; //fatal
    }

    if (!_invoke_msg_handler(msg, session, context)) goto error;

    //recurse thru batch of commands
    if (OeNetConn_available(OeSession_get_conn(session)) > 0 ) {
        _buffered_on_read(session);
    }

    quit:
    return;

    error:
    OeNet_cleanup_conn(session);
    return;
}

T OepServer_new(Oed_Dispatcher dispatcher, OeNet net) {

    assert(dispatcher);
    assert(net);

    T _this_;
    _this_ = Mem_calloc(1, sizeof *_this_, __FILE__, __LINE__);

    _this_->dispatcher  = dispatcher;
    _this_->net         = net;
    _this_->kernels = SKeyTable_new(100);

    OeNet_set_proto(net, _this_);
    OeNet_set_read_handler(net, _buffered_on_read);
    OeNet_set_con_timeout_cb(net, _con_timeout_cb);

    return _this_;
}

//todo: idgen is a per server thing.  rename this to add_proto and move idgen back to _new
void OepServer_configure(T _this_,
                         oep_client_read_msg  read_fun,
                         oep_client_write_msg write_fun, 
                         oe_id (* idfun) (void *),
                         void *idfun_arg ) {

    assert(!_this_->write_fun);
    assert(!_this_->read_fun);
    _this_->read_fun    = read_fun;
    _this_->write_fun   = write_fun;

    OeNet_set_idgen(_this_->net, idfun, idfun_arg);
}

int OepServer_listen(T _this_, char *hostifc, int port) {
    assert(_this_->write_fun);
    assert(_this_->read_fun);
    OeNet_listen_for_connect( _this_->net, hostifc, port);
    return 0;
}

void OepServer_stats(T _this_) {
    printf("todo OepServer stats\n");
}
void OepServer_free(T *_this_p) {
    assert(_this_p && *_this_p);
    T _this_ = *_this_p;
    //OeNet_stop(_this_->net);
    //OeNet_free(&_this_->net);
    Table_free(&_this_->kernels);
    Mem_free(_this_, __FILE__, __LINE__);
}

void OepServer_add_login_module(T _this_, LoginModule m) {
    if (_this_->security_context == NULL) {
        _this_->security_context = SecurityContext_new();
    }
    SecurityContext_add_module(_this_->security_context, m);
}

extern void OepServer_add_kernel(T _this_, oe_scalar extentname, OEK kernel) {
    assert(kernel);
    assert(extentname);
    char * extname_cpy = oec_ncpy_str(NULL, extentname, strlen(extentname));
    assert(extname_cpy);
    OE_DLOG( NULL, "OepServer_add_kernel for extname: %s\n", extname_cpy);
    Table_put(_this_->kernels, (void *) extname_cpy, kernel);
}

#undef T

