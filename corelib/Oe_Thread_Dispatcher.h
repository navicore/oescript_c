/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OE_THREAD_DISPATCHER_H
#define OE_THREAD_DISPATCHER_H

#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include "oe_common.h"
#include "oed_dispatcher_common.h"

#define T Oe_Thread_Dispatcher

#ifdef OE_USE_THREADS
enum TD_ITEM_TYPE {
    TD_PROCESS, TD_TQUIT, TD_DIAG
};

typedef struct td_item_ {
    enum TD_ITEM_TYPE type;
    user_callback *fun;
    user_callback_arg fun_args;
    struct td_item_ *next;
} td_item_;

struct td_dispatcher_queue {
    td_item_            *head;
    td_item_            *tail;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    int             running;
    long size;
    long counter;
};
typedef struct td_dispatcher_queue TD_QUE;
#endif

struct td_T {
#ifdef OE_USE_THREADS
    TD_QUE     *que;
    int       number_of_threads;
    int       running;
#endif
    pthread_t thread_id[MAX_THREADS];
};

typedef struct td_T *T;

extern void Oe_Thread_Dispatcher_exec (T, user_callback *, user_callback_arg);

extern int Oe_Thread_Dispatcher_start(T);

extern int Oe_Thread_Dispatcher_stop(T);

extern T Oe_Thread_Dispatcher_new(int);

extern void Oe_Thread_Dispatcher_free(T*);

extern int (Oe_Thread_Dispatcher_stats)(T _this_, FILE *stream);

#undef T

#endif //end OE_THREAD_DISPATCHER_H

#ifdef __cplusplus
}
#endif 

