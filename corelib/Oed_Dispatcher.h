/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OE_TIMER_DISPATCHER_H
#define OE_TIMER_DISPATCHER_H

#include <stdbool.h>
#include <stdio.h>
#include "oe_common.h"
#include "DataObject.h"
#include "DataObjectList.h"
#include "oed_dispatcher_common.h"

#define T Oed_Dispatcher
enum ITEM_TYPE {
    REG, UNREG, PROCESS, TQUIT, EXECUTE, TIMEOUT, HEARTBEAT, DIAG
};

struct dispatcher_T;

typedef struct item_ item_;
struct item_ {
    enum ITEM_TYPE type;
    oe_id sid;
    bool consume;
    bool free_alloc;
    bool run_on_timeout;
    oe_scalar *object;
    user_callback *match_handler;
    user_callback *timeout_handler;
    user_callback_arg args;
    int key_idx; //the index of the key used for the matcher's most-unique-key test
    char *key;    //used for clones that need to clean up themselves after orig is gone
    struct event *timer_event;
    oe_time duration;
    item_ *next;
    Set_T items;  //holder for self remove
    List_T group; //for groups of templates for AND and OR that are removed as a
                  //group when one of them hits or times out
    struct dispatcher_T *dispatcher;
};

typedef struct dispatcher_queue QUE;
struct dispatcher_queue {
    item_ *head;
    item_ *tail;

#ifdef OE_USE_THREADS
    pthread_mutex_t lock;
    pthread_cond_t  cond;
#endif

    bool running;
    long size;
    long counter;
};

struct dispatcher_T {
    QUE         *que;
    Table_T     items;
    const char  *name;

    pthread_t thread_id;
    size_t  number_of_threads;

    struct  event_base *net_obj;
    oe_id   next_unused_id;

    struct  event *notify_event;
    int     recv_wakeup_fd;   
    int     send_wakeup_fd;   

#ifdef OE_USE_THREADS
    pthread_mutex_t id_gen_lock;
    pthread_cond_t  id_gen_cond;
#endif
};

typedef struct dispatcher_T *T;

extern void Oed_Dispatcher_timed_exec( T, user_callback *, user_callback_arg, oe_time );

/*
    run a void function with a single arg in a dispatcher thread
*/
extern void Oed_Dispatcher_exec( T, user_callback *, user_callback_arg );

extern oe_id Oed_Dispatcher_reg_group(T, DataObjectList, oe_time, bool,
                                      user_callback *, user_callback *, user_callback_arg);

/*
    register a void function and single arg for processing when a
    match of a template arrives via a _process call
*/
extern oe_id Oed_Dispatcher_reg (T, DataObject, oe_time, bool, user_callback *,
                                 user_callback *, user_callback_arg);

/*
    remove the handler set in the _reg call
*/
extern void Oed_Dispatcher_unreg (T, oe_id sid);


/*
    pass a pattern to trigger the void functions registered in _reg
    to run if they were registered with a template matching the
    _process arg.
 
    matching rules:  all fields in the template must be found in
    the _process arg fields.  ie:
      template {one, three}
      matches _process arg {one, two, three, four}
      and matches {one, three}
      but does not match {two, three, four}
*/
extern void Oed_Dispatcher_get_name(T dispatcher, const char **name);

extern void Oed_Dispatcher_process(T, DataObject, bool);


extern int Oed_Dispatcher_start(T);

extern int Oed_Dispatcher_stop(T);

extern T Oed_Dispatcher_new(size_t, const char *);

extern void Oed_Dispatcher_free(T*);

extern void *Oed_Dispatcher_get_event_base(T);

extern int (Oed_Dispatcher_stats)(T _this_);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

