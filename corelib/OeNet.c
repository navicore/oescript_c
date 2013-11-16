/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "oec_values.h"
#include "set.h"
#include "Oec_IdSet.h"
#include "OeNetConn.h"
#include "OeSession.h"
#include "OeNet.h"
#include "oescript_core_resources.h"

#define T OeNet

T OeNet_new(void                       *proto,
            void                       *net_obj,
            OENET_listen_for_connect   *listen_fun,
            OENET_cleanup_conn         *cleanup_conn_fun,
            OENET_connect              *connect_fun,
            OENET_listen_for_bytes     *listen_for_bytes_fun,
            OENET_new_conn             *new_conn_fun) {
    assert(listen_fun);
    assert(cleanup_conn_fun);
    assert(connect_fun);
    assert(listen_for_bytes_fun);
    assert(new_conn_fun);
    T _this_;
    _this_ = Mem_calloc( 1, sizeof *_this_, __FILE__, __LINE__ );
    _this_->sessions = Oec_IdSet_new(100);
    _this_->proto = proto;
    _this_->net_obj = net_obj;
    _this_->listen_fun = listen_fun;
    _this_->cleanup_conn_fun = cleanup_conn_fun;
    _this_->connect_fun = connect_fun;
    _this_->listen_for_bytes_fun = listen_for_bytes_fun;
    _this_->new_conn_fun = new_conn_fun;
    return _this_;
}

void *OeNet_get_net_obj(T _this_) {
    assert(_this_);
    return _this_->net_obj;
}

/*
void OeNet_set_net_obj(T _this_, void *net_obj) {
    assert(_this_);
    assert(_this_->net_obj == NULL);
    _this_->net_obj = net_obj;
}
*/

oenet_callback *OeNet_get_con_timeout_cb(T _this_) {
    assert(_this_);
    return _this_->con_timeout_cb;
}

oenet_callback *OeNet_get_read_handler(T _this_) {
    assert(_this_);
    return _this_->read_handler;
}

void OeNet_free(T *objp) {
    assert(objp && *objp);
    T _this_ = *objp;
    OeNet_stop(_this_);//make sure
    assert(_this_);
    assert(_this_->sessions);
    Oec_IdSet_free(&_this_->sessions);
    if (_this_->name) {
        Mem_free( _this_->name, __FILE__, __LINE__ );
    }
    Mem_free( _this_, __FILE__, __LINE__ );
}

Oec_IdSet OeNet_get_sessions( T _this_ ) {
    assert(_this_);
    return _this_->sessions;
}

//todo: typedef for these func args
void OeNet_set_idgen( T _this_, oe_id (* idfun) ( void * ), void *idfun_arg ) {
    assert(_this_);
    assert(!_this_->create_id);
    assert(!_this_->create_id_arg);
    _this_->create_id      = idfun;
    _this_->create_id_arg  = idfun_arg;
}

oe_id OeNet_create_id( T _this_ ) {
    assert(_this_);
    if (_this_->create_id) {
        return _this_->create_id( _this_->create_id_arg );
    } else {
        _this_->lastid++;
        return _this_->lastid;
    }
}

void OeNet_stats(T _this_) {
    assert(_this_);
    printf("ejs Oenet stats\n");
    //todo: ejs call the wired _stats method
}

void OeNet_stop(T _this_) {
    assert(_this_);
}

void OeNet_set_read_handler(T _this_, oenet_callback *cb) {
    assert(_this_);
    _this_->read_handler = cb;
}

void OeNet_set_con_timeout_cb(T _this_, oenet_callback *cb) {
    assert(_this_);
    _this_->con_timeout_cb = cb;
}

void *OeNet_get_proto( T _this_ ) {
    assert(_this_);
    return _this_->proto;
}

void OeNet_set_proto(T _this_, void *proto) {
    assert(_this_);
    assert(_this_->proto == NULL);
    _this_->proto = proto;
}

//setup socket server
void OeNet_listen_for_connect(T _this_, char *hostifc, int port) {
    assert(_this_);
    _this_->listen_fun(_this_, hostifc, port);
}

//setup socket client
//OeSession OeNet_listen_for_bytes( T _this_, void *no ) {
void * OeNet_listen_for_bytes( T _this_, void *no ) {
    assert(_this_);

    return _this_->listen_for_bytes_fun(_this_, no);
}

//returns fd
void *OeNet_connect(T _this_, const char *hostname, int port) {
    assert(_this_);

    return _this_->connect_fun(_this_, hostname, port);
}

OeNetConn OeNet_new_conn(T _this_) {
    assert(_this_);
    return _this_->new_conn_fun(_this_);
}

//void OeNet_cleanup_conn(OeSession session) {
void OeNet_cleanup_conn(void * session) {
    assert(session);
    T _this_ = OeSession_get_net(session);
    assert(_this_);
    _this_->cleanup_conn_fun(session);
}

void OeNet_set_name(T _this_, oe_scalar name) {
    _this_->name = oec_cpy_str(NULL, name);
}
oe_scalar OeNet_get_name(T _this_) {
    return _this_->name;
}

#undef T

