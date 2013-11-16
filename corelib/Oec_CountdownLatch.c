/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include "Oec_CountdownLatch.h"
#include "oec_threads.h"
#include "mem.h"
#include "oec_values.h"

///
/// todo: add coroutine suppot there like there is in future
///

#define T Oec_CountdownLatch

struct T {
    int _count;
    int _max_count;
#ifdef OE_USE_THREADS
    pthread_mutex_t lock;
    pthread_cond_t  cond;
#endif
};

T Oec_CountdownLatch_new(void) {

    T _this_;
    _this_ = Mem_calloc(1, sizeof *_this_, __FILE__, __LINE__);
    oe_pthread_mutex_init(&_this_->lock, NULL);
    oe_pthread_cond_init(&_this_->cond, NULL);
    return _this_;
}

int Oec_CountdownLatch_free(T *_this_p) {

    assert(_this_p && *_this_p);
    T _this_ = *_this_p;
    int rc = Oec_CountdownLatch_get(_this_, OEC_FOREVER);
    if (rc) return rc;

    oe_pthread_mutex_destroy( &_this_->lock );
    oe_pthread_cond_destroy( &_this_->cond );
    Mem_free(_this_, __FILE__, __LINE__);

    return 0;
}

static int _update(T _this_, int value) {

    int ret = 0;
    oe_pthread_mutex_lock(&_this_->lock);
    _this_->_count += value;
    ret = _this_->_count;
    if (value > 0) {
        _this_->_max_count += value;
    }
    oe_pthread_cond_signal(&_this_->cond);
    oe_pthread_mutex_unlock(&_this_->lock);
    return ret;
}

int Oec_CountdownLatch_get_hi_water_mark(T _this_) {
    int ret = 0;
    oe_pthread_mutex_lock(&_this_->lock);
    ret = _this_->_max_count;
    oe_pthread_cond_signal(&_this_->cond);
    oe_pthread_mutex_unlock(&_this_->lock);
    return ret;
}
int Oec_CountdownLatch_increment(T _this_) {
    return _update(_this_, 1);
}

int Oec_CountdownLatch_decrement(T _this_) {
    return _update(_this_, -1);
}

//timeout is in milliseconds
int Oec_CountdownLatch_get( T _this_, oe_time timeout ) {

    if (timeout == OEC_FOREVER)  timeout = OEC_MONTH;

    int ret = -1;

    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct timespec ts;

    ts.tv_sec = ( timeout / 1000 ) + tv.tv_sec;
    long int nsecs = ( ( timeout % 1000 ) * 1000 ) + tv.tv_usec;
    ts.tv_sec += ( nsecs / 1000000 );
    ts.tv_nsec = ( nsecs % 1000000 );

    oe_time starttime = oec_get_time_milliseconds();
    oe_time stoptime = starttime + timeout;

    oe_pthread_mutex_lock( &_this_->lock );
    while (_this_->_count != 0) {
        oe_pthread_cond_timedwait(&_this_->cond, &_this_->lock, &ts);
        oe_time now = oec_get_time_milliseconds();
        if (now >= stoptime) break;
    }
    if (_this_->_count == 0) {
        ret = 1; //1 means success (you got the latch) <= 0 means fail
    }
    oe_pthread_mutex_unlock( &_this_->lock );

    return ret;
}

#undef T

