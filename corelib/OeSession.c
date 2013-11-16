/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "oe_common.h"
#include "OeNet.h"
#include "OeNetConn.h"
#include "OeSession.h"

#define T OeSession

T oesession_new_dummy() { //for testing
    T _this_;
    _this_ = Mem_calloc(1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = Arena_new();
    return _this_;
}
T oesession_new(void *net) {
    assert(net);
    T _this_ = oesession_new_dummy();
    _this_->net = (OeNet) net;
    _this_->start_time = oec_get_time_milliseconds();
    _this_->conn = OeNet_new_conn(net);
    return _this_;
}
void OeSession_free( T *objp) {
    T _this_ = (OeSession) *objp;
    assert(_this_);
    assert(_this_->conn);
    OeNetConn_free(&_this_->conn);
    Arena_T arena = _this_->arena;
    if (arena) {
        Arena_dispose(&arena);
    }
    Mem_free(_this_, __FILE__, __LINE__);
}

void *OeSession_get_net(T _this_) {
    return _this_->net;
}

OeNetConn OeSession_get_conn(T _this_) {
    return _this_->conn;
}

oe_time OeSession_get_starttime(T _this_) {
    return _this_->start_time;
}

bool OeSession_state_is_set( T _this_, oe_flags flag ) {
    return OEC_FLAG_ISSET(_this_->state, flag);
}
void OeSession_state_clear( T _this_, oe_flags flag ) {
    OEC_FLAG_CLEAR(_this_->state, flag);
}
void OeSession_state_set( T _this_, oe_flags flag ) {
    OEC_FLAG_SET(_this_->state, flag);
}

Arena_T OeSession_get_arena( T _this_ ) {
    return _this_->arena;
}

void OeSession_set_appl_context( T _this_, void *c ) {
    _this_->appl_context = c;
}
void *OeSession_get_appl_context( T _this_ ) {
    return _this_->appl_context;
}

void OeSession_set_sess_context( T _this_, void *c ) {
    _this_->sess_context = c;
}
void *OeSession_get_sess_context( T _this_ ) {
    return _this_->sess_context;
}

/*
void OeSession_set_protocol_context(T _this_, void *pc) {
    _this_->proto_context = pc;
}
void *OeSession_get_protocol_context(T _this_) {
    return _this_->proto_context;
}
*/

void OeSession_set_subject(T _this_, void *subject) {
    _this_->subject = subject;
}
void *OeSession_get_subject(T _this_) {
    return _this_->subject;
}

void OeSession_set_id(T _this_, oe_id sid) {
    _this_->sid = sid;
}
oe_id OeSession_get_id(T _this_) {
    return _this_->sid;
}

void OeSession_set_rb_on_disconnect(T _this_) {
    _this_->rb_on_dis = true;
}

bool OeSession_rb_on_disconnect(T _this_) {
    return _this_->rb_on_dis;
}

int OeSession_decr_task_count(T _this_) {
    return --_this_->outstanding_task_count;
}
int OeSession_incr_task_count(T _this_) {
    return ++_this_->outstanding_task_count;
}
int OeSession_get_task_count(T _this_) {
    return _this_->outstanding_task_count;
}

void OeSession_save_tid(T _this_, oe_id tid) {
    if (!_this_->pending_txns) {
        _this_->pending_txns = Set_new(1, NULL, NULL);
    }
    //ejs todo: stop casting tid to pntr, do it right and alloc it
    Set_put(_this_->pending_txns, (void *) tid);
}

oe_id OeSession_clear_tid(T _this_, oe_id tid) {
    if (!_this_->pending_txns) {
        return 0;
    }
    return(oe_id) Set_remove(_this_->pending_txns, (void *) tid);
}

Set_T OeSession_get_tids(T _this_) {
    return _this_->pending_txns;
}

#undef T

