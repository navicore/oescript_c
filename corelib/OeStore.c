/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include <stdbool.h>
//#include <stdio.h>
#include "DataObject.h"
#include "DataObjectList.h"
#include "OeStore.h"
#include "OeStoreHolder.h"
#include "oec_log.h"

#define T OeStore

char *(OeStore_strerror)(T _this_, int err) {
    return _this_->_strerror(_this_->store_impl, err);
}

int (OeStore_stop)(T _this_) {
    return _this_->_stop(_this_->store_impl);
}

int (OeStore_free)(T *_this_p) {
    T _this_ = *_this_p;
    return _this_->_oestore_free(&_this_->store_impl);
}

int (OeStore_stats)(T _this_) {
    return _this_->_stats(_this_->store_impl);
}

int (OeStore_deadlock_detect)(T _this_, int A) {
    return _this_->_deadlock_detect(_this_->store_impl, A);
}

void (OeStore_start_deadlock_watchdog)(T _this_, bool B) {
    _this_->_start_deadlock_watchdog(_this_->store_impl, B);
}

void (OeStore_set_fire_write_event_cb)(T _this_, user_fire_wevent_func *fun, user_fire_wevent_func_arg funarg) {
    OE_TLOG(0, "OeStore_set_fire_write_event_cb\n"); //i18n
    _this_->_set_fire_write_event_cb(_this_->store_impl, fun, funarg);
}

int (OeStore_lease_reaper)(T _this_) {
    return _this_->_lease_reaper(_this_->store_impl);
}

oe_id (OeStore_create_id)(T _this_) {
    return _this_->_create_id(_this_->store_impl);
}

int (OeStore_put)(T _this_, OeStoreHolder holder, Arena_T arena) {
    return _this_->_oestore_put(_this_->store_impl, holder, arena);
}

int (OeStore_get)(T _this_, OeStoreQuery query) {
    return _this_->_oestore_get(_this_->store_impl, query);
}

int (OeStore_start_txn)(T _this_, oe_dur dur, OESTORE_TXN *oetxn, Arena_T arena) {
    return _this_->_start_txn(_this_->store_impl, dur, oetxn, arena);
}

int (OeStore_update_txn)(T _this_, oe_id tid, oe_dur dur, OESTORE_TXN_STATUS oetxn, Arena_T arena, int enlisted) {
    return _this_->_update_txn(_this_->store_impl, tid, dur, oetxn, arena, enlisted);
}
/*
int (OeStore_query_txn)(T _this_, oe_id tid, oe_time *time, OESTORE_TXN_STATUS *oetxn, Arena_T arena) {
    return _this_->_query_txn(_this_->store_impl, tid, time, oetxn, arena);
}
*/

int (OeStore_update_lease)(T _this_, oe_id lid, oe_time time, Arena_T arena) {
    return _this_->_update_lease(_this_->store_impl, lid, time, arena);
}

T (OeStore_new)(void *store_impl,
                OESTORE_strerror                *_strerror,
                OESTORE_stop                    *_stop,
                OESTORE_free                    *_oestore_free,
                OESTORE_stats                   *_stats,
                OESTORE_deadlock_detect         *_deadlock_detect,
                OESTORE_start_deadlock_watchdog *_start_deadlock_watchdog,
                OESTORE_set_fire_write_event_cb *_set_fire_write_event_cb,
                OESTORE_lease_reaper            *_lease_reaper,
                OESTORE_create_id               *_create_id,
                OESTORE_put                     *_oestore_put,
                OESTORE_get                     *_oestore_get, 
                OESTORE_start_txn               *_start_txn,
                OESTORE_update_txn              *_update_txn,
                //OESTORE_query_txn               *_query_txn,
                OESTORE_update_lease            *_update_lease) {
    T _this_;

    _this_ = Mem_alloc(sizeof *_this_, __FILE__, __LINE__);
    _this_->store_impl = store_impl;

    _this_->_strerror                  = _strerror;                     
    _this_->_stop                      = _stop;                         
    _this_->_oestore_free              = _oestore_free;                         
    _this_->_stats                     = _stats;                        
    _this_->_deadlock_detect           = _deadlock_detect;              
    _this_->_start_deadlock_watchdog   = _start_deadlock_watchdog;      
    _this_->_set_fire_write_event_cb   = _set_fire_write_event_cb;
    _this_->_lease_reaper              = _lease_reaper;                 
    _this_->_create_id                 = _create_id;                    
    _this_->_oestore_put               = _oestore_put;                          
    _this_->_oestore_get               = _oestore_get;                          
    _this_->_start_txn                 = _start_txn;                    
    _this_->_update_txn                = _update_txn;                   
//    _this_->_query_txn                 = _query_txn;                    
    _this_->_update_lease              = _update_lease;                 

    return _this_;
}

#undef T

