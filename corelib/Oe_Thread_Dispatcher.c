/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "oe_common.h"
#include <errno.h>
#include <stdio.h>
#include "oec_threads.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "Oe_Thread_Dispatcher.h"
#include "oec_threads.h"

#define T Oe_Thread_Dispatcher

static void *_do_work(void *);

#ifdef OE_USE_THREADS
//blocking pop
static td_item_ *_que_pop(TD_QUE *que) {

    td_item_ *item;

    oe_pthread_mutex_lock(&que->lock);
    while (NULL == que->head) {
        oe_pthread_cond_wait(&que->cond, &que->lock);
    }
    item = que->head;
    que->head = item->next;
    if (NULL == que->head) {
        que->tail = NULL;
    }
    que->size--;
    oe_pthread_mutex_unlock(&que->lock);

    return item;
}

static void _que_push(T _this_, td_item_ *item) {

    assert(item);

    TD_QUE *que = _this_->que;

    //put in work queue
    item->next = NULL;

    oe_pthread_mutex_lock(&que->lock);

    if (NULL == que->tail) {
        que->head = item;
    } else {
        que->tail->next = item;
    }
    que->tail = item;
    que->size++;
    que->counter++;

    oe_pthread_cond_signal(&que->cond);
    oe_pthread_mutex_unlock(&que->lock);
}

static void _print_diag( T _this_ ) {
    TD_QUE *que = _this_->que;
    OE_ILOG(0, OEC_TDISPATCHER_STATS_1, que->size, que->counter);
}

static void *_do_work(void *arg) {

    T _this_ = (T) arg;

    TD_QUE *que = _this_->que;
    oe_pthread_mutex_lock(&que->lock);
    que->running++;
    oe_pthread_cond_signal(&que->cond);
    oe_pthread_mutex_unlock(&que->lock);

    for (;;) {

        td_item_ *item = _que_pop(_this_->que);

        if (item == NULL) continue;

        switch (item->type) {
        
        case TD_PROCESS:

            _process_fun(item->fun, item->fun_args);

            Mem_free(item, __FILE__, __LINE__);

            break;

        case TD_TQUIT:

            Mem_free(item, __FILE__, __LINE__);

            oe_pthread_exit( NULL );
            return NULL;

        case TD_DIAG:

            Mem_free(item, __FILE__, __LINE__);

            _print_diag(_this_);

            break;

        default:

            assert(0); //should never get here

            break;
        }
    }
    return NULL;
}
#endif

void Oe_Thread_Dispatcher_exec(T _this_, user_callback *fun, user_callback_arg args) {

#ifdef OE_USE_THREADS
    if (_this_->number_of_threads > 0) {

        //schedule it

        td_item_ *item;
        item = Mem_calloc(1, sizeof *item, __FILE__, __LINE__);
        item->type = TD_PROCESS;

        item->fun = fun;

        item->fun_args = args;

        _que_push(_this_, item);

    } else {
#endif
        //inline it
        _process_fun(fun, args);
#ifdef OE_USE_THREADS
    }
#endif
}

int Oe_Thread_Dispatcher_start(T _this_) {

OE_DLOG(NULL,"thread dispatcher starting...\n");
#ifdef OE_USE_THREADS
    pthread_attr_t attr;
    oe_pthread_attr_init(&attr);

    size_t nt = _this_->number_of_threads;
    if (nt > 0) {

        for (int i = 0; i < nt; i++) {
            int ret = oe_pthread_create( &_this_->thread_id[i], &attr, _do_work, _this_);
            if (ret) {
                OE_ERR(NULL, "can not create timer dispatcher thread");
                assert(0);
            }
        }
        bool ready = false;
        TD_QUE *que = _this_->que;
        while (!ready) {
            oe_pthread_mutex_lock(&que->lock);
            if (que->running >= nt) ready = true;
            oe_pthread_cond_signal(&que->cond);
            oe_pthread_mutex_unlock(&que->lock);
        }
    }
#endif
    _this_->running = 1;

    return 0;
}

int Oe_Thread_Dispatcher_stop(T _this_) {

#ifdef OE_USE_THREADS
    if (_this_->number_of_threads > 0 && _this_->running) {

        for (int i = 0; i < _this_->number_of_threads; i++) {
            td_item_ *item;
            item = Mem_calloc(1, sizeof *item, __FILE__, __LINE__);
            item->type = TD_TQUIT;

            _que_push(_this_, item);
        }

        for (int i = 0; i < _this_->number_of_threads; i++) {
            pthread_t tid = _this_->thread_id[i];
            oe_pthread_join( tid, NULL );
        }
    }
#endif
    _this_->running = 0;
    return 0;
}

T Oe_Thread_Dispatcher_new(int number_of_threads) {

#ifdef OE_NOTHREADS
    number_of_threads = 0;
#endif

    assert(number_of_threads <= MAX_THREADS);

    T _this_;
    _this_ = Mem_calloc(1, sizeof *_this_, __FILE__, __LINE__);

#ifdef OE_USE_THREADS
    TD_QUE *que;
    que = Mem_calloc(1, sizeof *que, __FILE__, __LINE__);
    oe_pthread_mutex_init(&que->lock, NULL);
    oe_pthread_cond_init(&que->cond, NULL);
    que->running = 0;
    _this_->que = que;
    _this_->number_of_threads = number_of_threads;
#endif
    _this_->running = 0;

    return _this_;
}

void Oe_Thread_Dispatcher_free(T *_this_p) {

    assert(_this_p && *_this_p);
    T _this_ = *_this_p;
    Oe_Thread_Dispatcher_stop(_this_); //make sure
#ifdef OE_USE_THREADS
    TD_QUE *que = _this_->que;
    oe_pthread_mutex_destroy( &que->lock );
    oe_pthread_cond_destroy( &que->cond );
    Mem_free(que, __FILE__, __LINE__);
#endif
    Mem_free(_this_, __FILE__, __LINE__);
}

int (Oe_Thread_Dispatcher_stats)(T _this_, FILE *stream) {

#ifdef OE_USE_THREADS
    if (_this_->number_of_threads > 0) {

        for (int i = 0; i < _this_->number_of_threads; i++) {
            td_item_ *item;
            item = Mem_calloc(1, sizeof *item, __FILE__, __LINE__);
            item->type = TD_DIAG;

            _que_push(_this_, item);
        }
    }
#endif
    return 0;
}

#undef T

