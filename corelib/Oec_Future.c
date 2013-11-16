/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include "Oec_Future.h"
#include "oec_threads.h"
#include "mem.h"
#include "arena.h"
#include "oec_values.h"
#include "oec_log.h"
#include "oescript_core_resources.h"

#define T Oec_Future

struct T {
    Arena_T arena;
    oe_id oeid;
    void *value;
    bool done;
    bool is_error;
    bool freed;
#ifdef OE_USE_THREADS
    pthread_mutex_t lock;
    pthread_cond_t  cond;
#endif
    void *alloc;
};

T Oec_Future_new(void) {

    T _this_;
    _this_ = Mem_calloc(1, sizeof *_this_, __FILE__, __LINE__);
    oe_pthread_mutex_init(&_this_->lock, NULL);
    oe_pthread_cond_init(&_this_->cond, NULL);
    return _this_;
}

int Oec_Future_free(T *_this_p) {

    int rc = 0;
    assert(_this_p && *_this_p);
    T _this_ = *_this_p;
    bool doit = true;
    oe_pthread_mutex_lock(&_this_->lock);
    if (!( _this_->done )) {
        OE_ERR(NULL, OEC_FUTURE_FREE_NOT_DONE);
        doit = false;
        rc = 1;
    }
    _this_->freed = true;
    oe_pthread_cond_signal(&_this_->cond);
    oe_pthread_mutex_unlock(&_this_->lock);

    if (doit) {
        void *a = _this_->alloc;
        if (a) {
            Mem_free(a, __FILE__, __LINE__);
        }
        oe_pthread_mutex_destroy( &_this_->lock );
        oe_pthread_cond_destroy( &_this_->cond );
        if (_this_->arena) {
            Arena_dispose(&_this_->arena);
        }
        Mem_free(_this_, __FILE__, __LINE__);
    }
    return rc;
}

bool Oec_Future_is_done(T _this_ ) {

    bool ret = false;
    oe_pthread_mutex_lock(&_this_->lock);
    ret = _this_->done;
    oe_pthread_cond_signal(&_this_->cond);
    oe_pthread_mutex_unlock(&_this_->lock);
    return ret;
}

static void _future_set(T _this_, void *value, oe_id oeid, bool is_error) {

    assert(!(_this_->done));
    bool freeme = false;
    oe_pthread_mutex_lock(&_this_->lock);
    if (_this_->freed) {
        freeme = true; //we're too late, someone tried to freemem already
    } else {
        if (value) {
            _this_->value = value;
            _this_->is_error = is_error;
        } else {
            _this_->oeid = oeid;
        }
        _this_->done = true;
    }
    oe_pthread_cond_signal(&_this_->cond);
    oe_pthread_mutex_unlock(&_this_->lock);
    if (freeme) {
        OE_DLOG(NULL, OEC_FUTURE_SET_BUT_PENDING_FREE);
        Oec_Future_free(&_this_);
    }
}
void Oec_Future_setid(T _this_, oe_id oeid) {
    _future_set(_this_, NULL, oeid, false);
}
void Oec_Future_set(T _this_, void *value) {
    _future_set(_this_, value, 0, false);
}
void Oec_Future_seterror(T _this_, void *value) {
    _this_->done = false;
    _future_set(_this_, value, 0, true);
}
bool Oec_Future_is_error(T _this_) {
    bool ret;
    oe_pthread_mutex_lock(&_this_->lock);
    ret = _this_->is_error;
    oe_pthread_cond_signal(&_this_->cond);
    oe_pthread_mutex_unlock(&_this_->lock);
    return ret;
}

void *Oec_Future_alloc( T _this_, long sz ) {
    //assert( !_this_->done );
    void *a = Mem_calloc(1, sz, __FILE__, __LINE__);
    _this_->alloc = a;
    return a;
}

//timeout is in milliseconds
void *Oec_Future_get( T _this_, oe_time timeout ) {

    if (timeout == OEC_FOREVER)  timeout = OEC_YEAR;

    assert(_this_);

    struct timeval tv;
    gettimeofday(&tv, NULL);

    void *retval;

    struct timespec ts;
    //ugh
    ts.tv_sec = ( timeout / 1000 ) + tv.tv_sec;
    long int nsecs = ( ( timeout % 1000 ) * 1000 ) + tv.tv_usec;
    ts.tv_sec += ( nsecs / 1000000 );
    ts.tv_nsec = ( nsecs % 1000000 );

    oe_time starttime = oec_get_time_milliseconds();
    oe_time stoptime = starttime + timeout;

    oe_pthread_mutex_lock( &_this_->lock );
    while (!_this_->done) {
    //while (!_this_->done && NULL == _this_->value && NULL == _this_->oeid) {
        oe_pthread_cond_timedwait(&_this_->cond, &_this_->lock, &ts);
        oe_time now = oec_get_time_milliseconds();
        if (now >= stoptime) break;
    }
    retval = _this_->value;
    oe_pthread_mutex_unlock( &_this_->lock );

    return retval;
}

//timeout is in milliseconds
oe_id Oec_Future_getid( T _this_, oe_time timeout ) {

    if (timeout == OEC_FOREVER)  timeout = OEC_YEAR;

    struct timeval tv;
    gettimeofday(&tv, NULL);

    oe_id retval;

    struct timespec ts;
    //ugh
    ts.tv_sec = ( timeout / 1000 ) + tv.tv_sec;
    long int nsecs = ( ( timeout % 1000 ) * 1000 ) + tv.tv_usec;
    ts.tv_sec += ( nsecs / 1000000 );
    ts.tv_nsec = ( nsecs % 1000000 );

    oe_time starttime = oec_get_time_milliseconds();
    oe_time stoptime = starttime + timeout;

    oe_pthread_mutex_lock( &_this_->lock );
    while (!_this_->done) {
    //while (!_this_->done && NULL == _this_->value && NULL == _this_->oeid) {
        oe_pthread_cond_timedwait( &_this_->cond, &_this_->lock, &ts );
        oe_time now = oec_get_time_milliseconds();
        if (now >= stoptime) break;
    }
    retval = _this_->oeid;
    oe_pthread_mutex_unlock( &_this_->lock );

    return retval;
}

void Oec_Future_set_arena(T _this_, Arena_T arena) {
    _this_->arena = arena;
}

#undef T

