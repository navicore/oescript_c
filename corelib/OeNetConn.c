/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "oec_values.h"
#include "mem.h"
#include "OeNetConn.h"
#include "string.h"

#define T OeNetConn

T OeNetConn_new(OENETCONN_free_conn_obj *free_conn_obj_fun,
                OENETCONN_get_pos       *get_pos_fun,
                OENETCONN_available     *available_fun,
                OENETCONN_read          *read_fun,
                OENETCONN_write         *write_fun,
                OENETCONN_peek          *peek_fun,
                OENETCONN_clear_timeout *clear_timeout_fun) {
    assert(free_conn_obj_fun && get_pos_fun && available_fun &&
           read_fun && write_fun && peek_fun && clear_timeout_fun);
    T _this_;
    _this_ = Mem_calloc(1, sizeof *_this_, __FILE__, __LINE__);
    _this_->free_conn_obj_fun   = free_conn_obj_fun;
    _this_->get_pos_fun         = get_pos_fun;
    _this_->available_fun       = available_fun;
    _this_->read_fun            = read_fun;
    _this_->write_fun           = write_fun;
    _this_->peek_fun            = peek_fun;
    _this_->clear_timeout_fun   = clear_timeout_fun;
    return _this_;
}

void OeNetConn_free(T *objp) {
    T _this_ = (T) *objp;
    OeNetConn_free_conn_obj(_this_);
    Mem_free(_this_, __FILE__, __LINE__);
    *objp = NULL;
}

void oenetconn_set_conn_obj( T _this_, void * e ) {
    assert(_this_);
    _this_->conn_obj = e;
}
void *oenetconn_get_conn_obj( T _this_ ) {
    assert(_this_);
    return _this_->conn_obj;
}

void oenetconn_set_timeout(T _this_, void* timeout) {
    assert(_this_);
    _this_->timeout = timeout;
}
void *oenetconn_get_timeout(T _this_) {
    assert(_this_);
    return _this_->timeout;
}

void OeNetConn_free_conn_obj(T _this_) {
    assert(_this_);
    _this_->free_conn_obj_fun(_this_->conn_obj);
}

int OeNetConn_get_pos(T _this_, char *needle, size_t needle_len) {
    assert(_this_);
    return _this_->get_pos_fun(_this_, needle, needle_len);
}

size_t OeNetConn_available(T _this_) {
    assert(_this_);
    return _this_->available_fun(_this_);
}

size_t OeNetConn_read(T _this_, void *data, size_t size) {
    assert(_this_);
    return _this_->read_fun(_this_, data, size);
}

int OeNetConn_write(T _this_, char *buffer, size_t size) {
    assert(_this_);
    return _this_->write_fun(_this_, buffer, size);
}

void *OeNetConn_peek(T _this_, size_t pos, size_t len) {
    assert(_this_);
    return _this_->peek_fun(_this_, pos, len);
}

void OeNetConn_clear_timeout(T _this_) {
    assert(_this_);
    _this_->clear_timeout_fun(_this_->timeout);
    _this_->timeout = NULL;
}

#undef T

