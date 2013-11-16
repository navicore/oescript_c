/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEC_THREADS_H
#define OEC_THREADS_H
#include <config.h>

#ifdef OE_USE_THREADS
///////////////////////////////////////
///build with support for threading ///
///////////////////////////////////////
#include <pthread.h>
#define oe_pthread_mutex_lock(lock)                 pthread_mutex_lock(lock)
#define oe_pthread_cond_timedwait(cond, lock, ts)   pthread_cond_timedwait( cond, lock, ts )
#define oe_pthread_mutex_unlock(lock)               pthread_mutex_unlock(lock)
#define oe_pthread_cond_signal(cond)                pthread_cond_signal(cond)
#define oe_pthread_mutex_init(lock, something)      pthread_mutex_init(lock, something)
#define oe_pthread_cond_init(cond, something)       pthread_cond_init(cond, something)
#define oe_pthread_mutex_destroy(lock)              pthread_mutex_destroy(lock)
#define oe_pthread_cond_destroy(cond)               pthread_cond_destroy(cond)
#define oe_pthread_mutex_timedlock(lock, ts)        pthread_mutex_timedlock(lock, ts)
#define oe_pthread_cond_wait(cond, lock)            pthread_cond_wait( cond, lock )
#define oe_pthread_attr_init(attr)                  pthread_attr_init(attr)
#define oe_pthread_create(i, a, f, d)               pthread_create(i, a, f, d)
#define oe_pthread_join(tid, athing)                pthread_join(tid, athing)
#define oe_pthread_detach(tid)                      pthread_detach(tid)
#define oe_pthread_exit( something )                pthread_exit( something )
#endif

#ifdef OE_NOTHREADS
//////////////////////////////////////
///strip out support for threading ///
//////////////////////////////////////
#define oe_pthread_mutex_lock(lock)                 0;
#define oe_pthread_cond_timedwait(cond, lock, ts)   0;
#define oe_pthread_mutex_unlock(lock)               0;
#define oe_pthread_cond_signal(cond)                0;
#define oe_pthread_mutex_init(lock, something)      0;
#define oe_pthread_cond_init(cond, something)       0;
#define oe_pthread_mutex_destroy(lock)              0;
#define oe_pthread_cond_destroy(cond)               0;
#define oe_pthread_mutex_timedlock(lock, ts)        0;
#define oe_pthread_cond_wait(cond, lock)            0;
#define oe_pthread_attr_init(attr)                  0;
#define oe_pthread_create(i, a, f, d)               0;
#define oe_pthread_join(tid, athing)                0;
#define oe_pthread_detach(tid)                      0;
#define oe_pthread_exit( something )                0;
#endif

#endif

#ifdef __cplusplus
}
#endif 

