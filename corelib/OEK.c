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
#include "OEK.h"

#include "OEKI.h"
#include "OEK_impl.h"
#include "oescript_core_resources.h"


#define T OEK

T (OEK_new) (void *_client,
             OEK_kernel_ctx_call  *_oek_impl_put,
             OEK_kernel_ctx_call  *_oek_impl_get,
             OEK_kernel_ctx_call  *_oek_impl_sta,
             OEK_kernel_ctx_call  *_oek_impl_upt,
             OEK_kernel_ctx_call  *_oek_impl_upl,
             OEK_kernel_free_call *_oek_impl_free,
             OEK_kernel_call      *_oek_impl_stop,
             OEK_kernel_call      *_oek_impl_stats) {
    assert(_client); assert(_oek_impl_put); assert(_oek_impl_get); assert(_oek_impl_sta); assert(_oek_impl_upt); assert(_oek_impl_upl); assert(_oek_impl_free);
    T _this_;
    _this_ = Mem_calloc(1, sizeof *_this_, __FILE__, __LINE__);
    _this_->_put = _oek_impl_put;
    _this_->_get = _oek_impl_get;
    _this_->_sta = _oek_impl_sta;
    _this_->_upt = _oek_impl_upt;
    _this_->_upl = _oek_impl_upl;
    _this_->_free = _oek_impl_free;
    _this_->_client = _client;
    _this_->_stop = _oek_impl_stop; //optional
    _this_->_stats = _oek_impl_stats; //optional

    return _this_;
}

void OEK_free(T *_this_p) {
    assert(_this_p && *_this_p);
    T _this_ = *_this_p;
    OEK_stop(_this_);//make sure
    _this_->_free(_this_p);
}

void (OEK_get)( T _this_, OEK_Context *context ) {
    assert(_this_);
    assert(_this_->_get);
    _this_->_get(_this_, context);
}

void (OEK_put)( T _this_, OEK_Context *context ) {
    assert(_this_);
    assert(_this_->_put);
    assert(context);
    _this_->_put(_this_, context);
}

void (OEK_sta)( T _this_, OEK_Context *context ) {
    assert(_this_);
    assert(_this_->_sta);
    _this_->_sta(_this_, context);
}

void (OEK_upt)( T _this_, OEK_Context *context ) {
    assert(_this_);
    assert(_this_->_upt);
    _this_->_upt(_this_, context);
}

void (OEK_upl)( T _this_, OEK_Context *context ) {
    assert(_this_);
    assert(_this_->_upl);
    _this_->_upl(_this_, context);
}
void *OEK_get_client(T _this_) {
    assert(_this_);
    return _this_->_client;
}

void (OEK_stop)(T _this_) {
    if (_this_->_stop) _this_->_stop(_this_);
}

void (OEK_stats)(T _this_) {
    printf("ejs oek calling impl stats\n");
    if (_this_->_stats) _this_->_stats(_this_);
}

void (OEK_exec)( T _this_, OEK_Context *context ) {

    assert(_this_);
    assert(context);

    OekMsg cmd_msg = context->cmd_msg;
    OEK_MSG_TYPE mtype = OekMsg_get_type(cmd_msg);

    switch (mtype) {
    case GET_CMD:
        //GetCmd cmd = OekMsg_get_get_cmd(cmd_msg);
        //context->expire_time = oec_get_time_milliseconds() + GetCmd_get_timeout(cmd);
        OEK_get(_this_, context);
        break;
    case PUT_CMD:
        OEK_put(_this_, context);
        break;
    case STA_CMD:
        OEK_sta(_this_, context);
        break;
    case UPT_CMD:
        OEK_upt(_this_, context);
        break;
    case UPL_CMD:
        OEK_upl(_this_, context);
        break;
    case CON_CMD:
    case DIS_CMD:
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
        break;
    }
}

#undef T

