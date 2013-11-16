/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "oe_common.h"
#include "DataObject.h"
#include "oec_threads.h"
#include <errno.h>
#include <stdio.h>
#include <event.h>
#include <unistd.h>
#include <string.h>
#include "Oed_Dispatcher.h"
#include "Iterator.h"
#include "SKeyTable.h"

#define T Oed_Dispatcher
#define BATCH_SZ 10

//////////////////////////////////////////////
//////////////////////////////////////////////
/// if there are no threads supported,     ///
/// then schedule/process calls should run ///
/// the process_que code with the 1 item   ///
/// instead of compiling in the que code   ///
/// at all.                                ///
///                                        ///
/// if threads are compiled-in but the     ///
/// dispatcher has been created with 0     ///
/// threads, you still must put things in  ///
/// the work que to insure against         ///
/// deadlocks.                             ///
///                                        ///
/// you can't know if you are in the       ///
/// dispatcher's own main loop holding     ///
/// locks or not when you call process or  ///
/// reg.                                   ///
//////////////////////////////////////////////
//////////////////////////////////////////////

static bool _processSet( Table_T items, Set_T itemset, item_ *item, int key_idx );
static int _add_template(T _this_, item_ *item);
static int _itemcmp(const void *x, const void *y);
static item_ *_create_reg_item(T _this_, oe_scalar *templ_p, oe_time dur, bool consume, user_callback *match_handler, user_callback *timeout_handler, user_callback_arg args); 
static item_ *_que_pop(QUE *que);
static oe_id _gen_id(T _this_);
static unsigned _itemhash(const void *x);
static void *_do_work(void *arg);
static void _free_object(oe_scalar *object);
static void _heartbeat_cb(int fd, short event, void *arg);
static void _print_diag( T _this_ );
static void _process(Table_T items, item_ *item);
static void _process_que(int fd, short which, void *arg);
static void _process_que_item(T _this_, item_ *item );
static void _que_push(T _this_, item_ *item);
static void _remove_group(Table_T items, List_T group, bool timeout);
static void _remove_item(Table_T, item_ *, bool);
static void _schedule_heartbeat( T _this_ );
static void _schedule_item(T _this_, item_ *item );
static void _timed_exec_cb( int fd, short event, void *arg );
static void _timeout_cb(int fd, short event, void *arg);
static void _watch_for_timeout( item_ *item, struct event_base *ev_base);

////////////
/// impl ///
////////////

static void _remove_group(Table_T items, List_T group, bool timeout) {
    if (group == NULL) return;
    item_ **gitems = (item_ **) List_toArray(group, NULL);
    if (gitems == NULL) return;
    size_t sz = List_length(group);
    for (int i = 0; i < sz; i++) {
        item_ *gitem = gitems[i];
        gitem->group = NULL; 
        if (i > 0) {
            //don't run 'em twice
            gitem->match_handler = NULL;
            gitem->timeout_handler = NULL;
        }
        _remove_item(items, gitem, timeout);
    }
    Mem_free(gitems, __FILE__, __LINE__);
    List_free(&group);
}

static void _remove_all_items(Table_T items) {
}
static void _remove_item(Table_T items, item_ *rmitem, bool timeout) {

    if (rmitem == NULL) {
        return;
    }
    if (rmitem->group) {
        // get each item from the list, null out the list field and
        // recursively call this until the whole set is removed
        _remove_group(items, rmitem->group, timeout);
        return;
    }

    Set_T s = rmitem->items;
    item_ *item = NULL;
    if (s) {
        item = Set_remove(s, (void *) rmitem); //if this was a dummy item from a timeout, get the real one
    }
    if (!item) return;

    if (item->timer_event) {
        timeout_del(item->timer_event);
        Mem_free(item->timer_event, __FILE__, __LINE__);
    }

    if ( timeout && item->timeout_handler!=NULL ) {
        _process_fun(item->timeout_handler, item->args);
    }
    if (rmitem != item) {
        Mem_free(rmitem, __FILE__, __LINE__); //a clone
    }

    if (item->key_idx) {
        oe_scalar key = item->object[item->key_idx];
        if (s) 
        if (key && Set_length(s) <= 0) {
            if (items) {
                Table_remove(items, key);
            }
            if (s) {
                Set_free( &s );
            }
        }
    }
    Mem_free(item, __FILE__, __LINE__);
}

#ifdef OE_USE_THREADS
static void _que_push(T _this_, item_ *item) {
    assert(_this_);
    assert(item);

    QUE *que = _this_->que;
    assert(que);

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

    //wake up worker thread
    if (write(_this_->send_wakeup_fd, "", 1) != 1) {
        OE_ERR(NULL, "Writing to thread notify pipe");
    }
}

//non-blocking pop
static item_ *_que_pop(QUE *que) {
    assert(que);
    item_ *item;

    oe_pthread_mutex_lock(&que->lock);
    item = que->head;
    if (NULL != item) {
        que->head = item->next;
        if (NULL == que->head) {
            que->tail = NULL;
        }
    }
    que->size--;
    oe_pthread_mutex_unlock(&que->lock);

    return item;
}
#endif

static oe_id _gen_id(T _this_) {

    assert(_this_);

    oe_id newid;

    oe_pthread_mutex_lock(&_this_->id_gen_lock);

    newid = _this_->next_unused_id++;

    oe_pthread_cond_signal(&_this_->id_gen_cond);
    oe_pthread_mutex_unlock(&_this_->id_gen_lock);

    return newid;
}

void Oed_Dispatcher_get_name(T _this_,  const char **name) {
    *name = _this_->name;
}

void *Oed_Dispatcher_get_event_base( T _this_ ) {

    assert(_this_);
    assert(_this_->net_obj);
    return _this_->net_obj;
}

typedef struct ___timed_holder {

    void *args;
    void (* fun) (void *);

} THOLDER;

static void _timed_exec_cb( int fd, short event, void *arg ) {

    THOLDER *holder = (THOLDER *) arg;
    assert(holder);
    assert(holder->args);
    assert(holder->fun);

    _process_fun(holder->fun, holder->args);

    Mem_free( holder, __FILE__, __LINE__ );
}

void Oed_Dispatcher_timed_exec( T _this_, user_callback *fun, user_callback_arg args, oe_time millis ) {

    assert(_this_);
    assert(fun);
    assert(args);

    struct timeval tv;
    struct event *timeout;

    timeout = Mem_calloc( 1, sizeof *timeout, __FILE__, __LINE__ );
    event_base_set( Oed_Dispatcher_get_event_base( _this_ ), timeout );

    THOLDER *holder;
    holder = Mem_calloc( 1, sizeof *holder, __FILE__, __LINE__ );
    holder->args = args;
    holder->fun = fun;

    evtimer_set( timeout, _timed_exec_cb, holder );

    evutil_timerclear(&tv);
    tv.tv_sec = millis / 1000;
    tv.tv_usec = (millis % 1000) * 1000;

    event_add(timeout, &tv);
}

static void _schedule_item(T _this_, item_ *item) {
#ifdef OE_USE_THREADS
    if (_this_->number_of_threads > 0) {
        _que_push(_this_, item);
        //oe_pthread_join( _this_->thread_id, NULL );
    } else {
        _process_que_item(_this_,item);
        _this_->que->counter++;
    }
#else
    _process_que_item(_this_,item);
#endif
}

void Oed_Dispatcher_exec( T _this_, user_callback *fun, user_callback_arg args ) {

    assert(_this_);
    item_ *item;
    item = Mem_calloc(1, sizeof *item, __FILE__, __LINE__);
    item->type = EXECUTE;
    item->dispatcher = _this_;

    item->match_handler = fun;

    item->args = args;

    _schedule_item(_this_, item);
}

static item_ *_create_reg_item(T _this_,
                               oe_scalar *templ_p,
                               oe_time dur,
                               bool consume,
                               user_callback *match_handler,
                               user_callback *timeout_handler,
                               user_callback_arg args) {
    assert(_this_);

    //todo: use pool of items and stop alloc'ing
    item_ *item;
    item = Mem_calloc(1, sizeof *item, __FILE__, __LINE__);
    item->dispatcher = _this_;

    oe_id sid = _gen_id(_this_);
    item->sid = sid;
    item->duration = dur;

    item->consume = consume;

    item->type = REG;

    item->object = templ_p;

    item->match_handler = match_handler;
    item->timeout_handler = timeout_handler;
    item->args = args;
    item->items = NULL;

    return item;
}

oe_id Oed_Dispatcher_reg(T _this_,
                         DataObject templ,
                         oe_time dur,
                         bool consume,
                         user_callback *match_handler,
                         user_callback *timeout_handler,
                         user_callback_arg args) {

    oe_scalar *templ_array = DataObject_toArray(templ);
    item_ *item = _create_reg_item(_this_, templ_array, dur, consume, match_handler, timeout_handler, args);
    oe_id sid = item->sid;

    _schedule_item(_this_, item);
    return sid;
}

/* all is an Oec_AList of DataObject entries */
oe_id Oed_Dispatcher_reg_group(T _this_,
                               DataObjectList all,
                               oe_time dur,
                               bool consume,
                               user_callback *match_handler,
                               user_callback *timeout_handler,
                               user_callback_arg args) {

    oe_id sid = 0;

    assert(all);
    List_T group = List_list(NULL);
    for (Iterator iter = DataObjectList_iterator(all, true);
        Iterator_hasMore(iter);) {
        DataObject o = Iterator_next(iter);
        oe_scalar *templ_p = DataObject_toArray(o);
        item_ *item = _create_reg_item(_this_, templ_p, dur, consume, match_handler, timeout_handler, args);
        sid = item->sid;
        group = List_append(group, List_list(item, NULL));
    }
    item_ **groupitems = (item_**) List_toArray(group, NULL);
    for (int i = 0; i < List_length(group); i++) {
        groupitems[i]->group = group;

        _schedule_item(_this_, groupitems[i]);
    }

    Mem_free(groupitems, __FILE__, __LINE__);

    return sid; //you can cancel the whole group by unreg'ing this one sid
}

void Oed_Dispatcher_unreg (T _this_, oe_id sid) {
    assert(_this_);
    item_ *item;
    item = Mem_calloc(1, sizeof *item, __FILE__, __LINE__);
    item->dispatcher = _this_;
    item->type = UNREG;
    item->sid = sid;
    item->items = NULL;

    _schedule_item(_this_, item);
}

void _clone_attr_array( oe_scalar *src, oe_scalar**clone_p ) {

    int sz;
    for (sz = 0; src[sz]; sz++) {
    }
    oe_scalar *clone;
    clone = Mem_alloc((sz + 1) * sizeof (char *), __FILE__, __LINE__);

    int i;
    for (i = 0; i < sz; i++) {
        oe_scalar str;
        str = Mem_alloc((strlen((char *) src[i]) + 1), __FILE__, __LINE__);
        strcpy((char *) str, (char *) src[i]);
        clone[i] = str;
    }
    clone[i] = NULL;

    *clone_p = clone;
}

void Oed_Dispatcher_process(T _this_, DataObject props, bool make_clone) {

    assert(_this_);
    OE_TLOG(0, "Oed_Dispatcher_process\n"); //i18n
    item_ *item;
    item = Mem_calloc(1, sizeof *item, __FILE__, __LINE__);
    item->dispatcher = _this_;
    oe_id sid = _gen_id(_this_);
    item->sid = sid;
    item->type = PROCESS;
    oe_scalar *propsarray = DataObject_toArray(props);
    if (make_clone) {
        oe_scalar *clone;
        _clone_attr_array(propsarray, &clone);
        item->object = clone;
    } else {
        item->object = propsarray;
    }
    item->free_alloc = make_clone; //if this is clone, the matcher must free it
    item->items = NULL;

    _schedule_item(_this_, item);
}

static void _heartbeat_cb(int fd, short event, void *arg) {

    item_ *item = ( item_ * )arg;
    T _this_ = item->dispatcher;

    _schedule_item(_this_, item);
}

static void _print_diag( T _this_ ) {
    QUE *que = _this_->que;
    OE_ILOG(0, OEC_DISPATCHER_STATS_1, _this_->name, que->size, que->counter);
}

static void _schedule_heartbeat( T _this_ ) {

    item_ *timer_item;
    timer_item = Mem_calloc( 1, sizeof *timer_item, __FILE__, __LINE__ );
    oe_id sid = _gen_id(_this_);
    timer_item->sid = sid;
    timer_item->dispatcher = _this_;
    timer_item->type = HEARTBEAT;

    struct timeval tv;
    struct event *timeout;

    timeout = Mem_calloc(1, sizeof *timeout, __FILE__, __LINE__);
    event_base_set(_this_->net_obj, timeout);
    timer_item->timer_event = timeout;

    evtimer_set(timeout, _heartbeat_cb, timer_item);

    evutil_timerclear(&tv);
    tv.tv_sec = 10;

    event_add(timeout, &tv);
}

#ifdef OE_USE_THREADS
static void _process_que(int fd, short which, void *arg) {

    //clear the pipe
    char buf[BATCH_SZ];
    int event_count = read(fd, buf, BATCH_SZ);
    if (event_count < 1 ) {
        OE_ERR(NULL, "Can't read libevent notify pipe");
        assert(0);
    }

    T _this_ = (T) arg;

    for (int c = 0; c < event_count; c++) {
        item_ *item = _que_pop(_this_->que);
        _process_que_item(_this_, item);
    }
}
#endif

static void *_do_work(void *arg) {

    T _this_ = (T) arg;
    assert(_this_);

    _this_->notify_event = Mem_calloc(1, sizeof *(_this_->notify_event), __FILE__, __LINE__);
#ifdef OE_USE_THREADS
    event_set(_this_->notify_event,_this_->recv_wakeup_fd,
              EV_READ | EV_PERSIST, _process_que, _this_);
#else
    static void *_dummy(void *arg) {return;}
    event_set(_this_->notify_event,_this_->recv_wakeup_fd, //event_loop will not run just for
              EV_READ | EV_PERSIST, _dummy, _this_);       //timers, this dummy pipe watching keeps
                                                           //it alive
#endif

    event_base_set(_this_->net_obj, _this_->notify_event);

    if (event_add(_this_->notify_event, 0) == -1) {
        OE_ERR(NULL, "Can't monitor libevent notify pipe");
        assert(0);
    }

#ifdef OE_USE_THREADS
    _schedule_heartbeat(_this_);

    QUE *que = _this_->que;
    oe_pthread_mutex_lock(&que->lock);
    que->running = true;
    oe_pthread_cond_signal(&que->cond);
    oe_pthread_mutex_unlock(&que->lock);
#endif
    event_base_loop(_this_->net_obj, 0);

    return NULL;
}

int Oed_Dispatcher_start(T _this_) {

    assert(_this_);
    OE_DLOG(NULL,"dispatcher '%s' starting...\n", _this_->name);

#ifdef OE_USE_THREADS
    if (_this_->number_of_threads > 0) {

        pthread_attr_t attr;
        oe_pthread_attr_init(&attr);
        int ret = oe_pthread_create( &_this_->thread_id, &attr, _do_work, _this_);
        if (ret) {
            OE_ERR(NULL,"can not crate thread");
            assert(0);
        }
        bool ready = false;
        QUE *que = _this_->que;
        while (!ready) {
            oe_pthread_mutex_lock(&que->lock);
            if (que->running) ready = true;
            oe_pthread_cond_signal(&que->cond);
            oe_pthread_mutex_unlock(&que->lock);
        }

    } else {
        //inline
        _do_work(_this_);
    }
#else
    _do_work(_this_);
#endif

    return 0;
}

int Oed_Dispatcher_print_diag(T _this_) {

    assert(_this_);

    item_ *item;
    item = Mem_calloc(1, sizeof *item, __FILE__, __LINE__);
    item->dispatcher = _this_;
    item->type = DIAG;
    oe_id sid = _gen_id(_this_);
    item->sid = sid;

    _schedule_item(_this_, item);

    //todo: find a way to free mem from pending timer events, or flush them somehow
    //}
    return 0;
}

int Oed_Dispatcher_stop(T _this_) {

    assert(_this_);

    item_ *item;
    item = Mem_calloc(1, sizeof *item, __FILE__, __LINE__);
    item->dispatcher = _this_;
    item->type = TQUIT;
    oe_id sid = _gen_id(_this_);
    item->sid = sid;

#ifdef OE_USE_THREADS
    if (_this_->number_of_threads > 0) {
        _que_push(_this_, item);
        oe_pthread_join( _this_->thread_id, NULL );
    } else {
        _process_que_item(_this_,item);
    }
#else
    _process_que_item(_this_,item);
#endif

    //todo: find a way to free mem from pending timer events, or flush them somehow
    //}
    return 0;
}

static int _itemcmp(const void *x, const void *y) {
    item_ *xitem = (item_ *) x;
    item_ *yitem = (item_ *) y;
    oe_id xint = (oe_id) xitem->sid;
    oe_id yint = (oe_id) yitem->sid;
    if (xint == yint) return 0;
    if (xint < yint) return -1;
    return 1;
}
static unsigned _itemhash(const void *x) {
    item_ *xitem = (item_ *) x;
    oe_id xint = (oe_id) xitem->sid;
    return(unsigned) xint;
}

T Oed_Dispatcher_new(size_t number_of_threads, const char *name) {

#ifdef OE_NOTHREADS
    number_of_threads = 0;
#endif

    assert(number_of_threads <= 1); //timer dispatcher can only have 1 thread 
                                    //for async or 0 for inline

    T _this_;
    _this_ = Mem_calloc(1, sizeof *_this_, __FILE__, __LINE__);

    if (name) {
        _this_->name = oec_cpy_str(NULL, (oe_scalar) name);
    }

    struct event_base *ev_base = event_init();
    assert(ev_base);
    _this_->net_obj = ev_base;

    QUE *que;
    que = Mem_calloc(1, sizeof *que, __FILE__, __LINE__);
    oe_pthread_mutex_init(&que->lock, NULL);
    oe_pthread_cond_init(&que->cond, NULL);
    que->running = false;
    _this_->que = que;
    _this_->number_of_threads = number_of_threads;

    _this_->next_unused_id = 101;
    oe_pthread_mutex_init(&_this_->id_gen_lock, NULL);
    oe_pthread_cond_init(&_this_->id_gen_cond, NULL);

    //set up wake up pipe(s)
    int fds[2];
    if (pipe(fds)) {
        OE_ERR(NULL, "Can't create notify pipe");
        assert(0);
    }
    _this_->recv_wakeup_fd = fds[0];
    _this_->send_wakeup_fd = fds[1];
    _this_->items = SKeyTable_new(1000);

    return _this_;
}

static void _free_items(Table_T items) {

    assert(items);

    int sz = Table_length(items);
    if (sz > 0) {
        void **array = Table_toArray(items, NULL);
        for ( int i = 0; i < sz && array[i]; i+= 2 ) {
            char *key = (char *) array[i];
            Set_T titem = Table_remove(items, key);
            if (titem) {
                Set_free( &titem );
            }
        }
        Mem_free(array, __FILE__, __LINE__);
    }
}

void Oed_Dispatcher_free(T *_this_p) {

    assert(_this_p && *_this_p);
    T _this_ = *_this_p;

    if (_this_->name) {
        Mem_free((void *) _this_->name, __FILE__, __LINE__);
    }

    Table_T items = _this_->items;
    Table_free( &items );

    event_base_free( (struct event_base *) Oed_Dispatcher_get_event_base(_this_) );

#ifdef OE_USE_THREADS
    Mem_free( _this_->notify_event, __FILE__, __LINE__);
#endif
    Mem_free( _this_->que,          __FILE__, __LINE__);
    Mem_free( _this_,               __FILE__, __LINE__);
}

//calculate the most unique fld in item against the map of lists of templates
//put the tuple in that list associated with the most unique key
static int _add_template(T _this_, item_ *item) {

    Table_T items = _this_->items;

    //calc most unique key
    oe_scalar key = NULL;
    int key_idx = 0;
    int set_size = 0;
    Set_T curset = NULL;
    for ( int i = 0; item->object[i]; i++ ) {
        oe_scalar tmpkey = item->object[i];
        if (strcmp("_", tmpkey) == 0) continue; //don't index by wildcard

        Set_T s = (Set_T) Table_get( items, tmpkey );
        if (s) {
            int size = Set_length( s );
            if ( !key || size < set_size ) {
                key = tmpkey;
                key_idx = i;
                set_size = size;
                curset = s;
            }
        } else { //found an unused key
            key = tmpkey;
            key_idx = i;
            set_size = 0;
            curset = Set_new( 10, _itemcmp, _itemhash );
            Table_put( items, key, curset );
            break;
        }
    }
    if (!curset) {
        OE_ERR(0, "Dispatcher can not add template, make sure it is not all '_' fields\n"); //i18n
        return -1;
    }
    item->items = curset;
    item->key_idx = key_idx; //for self remove of empty sets
    Set_put( curset, item );
    return 0;
}

static void _timeout_cb(int fd, short event, void *arg) {

    item_ *item = ( item_ * )arg;
    T _this_ = item->dispatcher;
    _remove_item(_this_->items, item, true);
}

static void _watch_for_timeout( item_ *item, struct event_base *ev_base) {

    if ( item->duration == OEC_FOREVER ) return;

    T _this_ = item->dispatcher;

    struct timeval tv;
    struct event *timeout;

    timeout = Mem_calloc(1, sizeof *timeout, __FILE__, __LINE__);
    event_base_set(ev_base, timeout);
    item->timer_event = timeout;

    evtimer_set(timeout, _timeout_cb, item);

    evutil_timerclear(&tv);
    long sec, msec, dur;
    dur = item->duration;
    sec = dur / 1000;
    if (sec) {
        msec = dur - (sec * 1000);
    } else {
        msec = dur;
    }

    tv.tv_sec = sec;
    tv.tv_usec = msec * 1000;

    event_add(timeout, &tv);
}

static void _free_object(oe_scalar *object) {

    for ( int i = 0; object[i]; i++ ) {
        Mem_free( object[i], __FILE__, __LINE__ );
    }

    Mem_free(object, __FILE__, __LINE__);
}

//todo: skip testing the idx keys of both item and titem
static bool _processSet( Table_T items, Set_T itemset, item_ *item, int key_idx ) {

    OE_TLOG(0, "Oed_Dispatcher _processSet\n"); //i18n
    assert(item);
    assert(items);
    assert(itemset);

    bool done = false;

    //when you get a match, remove template and invoke callback

    void **array = Set_toArray(itemset, NULL);
    for ( int i = 0; array[i]; i++ ) {
        OE_TLOG(0, "Oed_Dispatcher _processSet template %i\n", i); //i18n
        item_ *titem = (item_ *) array[i];
        int titem_key_idx = titem->key_idx;
        bool matched = true;
        for (int j = 0; matched && titem->object[j]; j++) {
            if (j == titem_key_idx) continue; //this was the key that put this 
                                              // template into this set, don't 
                                              // re-test it
            char *tobj = (char *) titem->object[j];

            bool kmatched = false;
            for (int k = 0; !kmatched && item->object[k]; k++) {
                if (k == key_idx) continue; //this was the key that got us the set, 
                                            // don't re-test it
                char *iobj = (char *) item->object[k];
                OE_TLOG(0, "               _processSet template %i fld: %s tst: %s\n", 
                        i, iobj, tobj); //i18n
                if (strcmp(iobj, tobj) == 0) {
                    kmatched = true;
                } else
                    if (strcmp("_", tobj) == 0) kmatched = true; //wildcard
            }
            matched = kmatched;
        }
        if (matched) {
            OE_TLOG(0, "               _processSet matched %i\n", i); //i18n
            //a match
            if (titem->match_handler) {
                OE_TLOG(0, "               _processSet matched running handler %i\n", i); //i18n
                _process_fun(titem->match_handler, titem->args);
                //this might happen if someone registered
                //something that should only timeout like
                //a timer but idiotically specified a
                //possible pattern and the pattern hit.
            }
            bool consume = titem->consume;
            // note, this is the point to implement persistent subs
            _remove_item(items, titem, false);  //always remove the item.
            if (consume) {
                done = true;
                break; //must be a take
            }
        }
    }
    Mem_free(array, __FILE__, __LINE__);

    return done;
}

// for each fld in item, get the list from items and test each member in items against item
static void _process(Table_T items, item_ *item) {
    for (int i = 0; item->object[i]; i++) {
        const oe_scalar key = item->object[i];
        Set_T s = (Set_T) Table_get(items, key);
        if (!s) continue;
        bool done = _processSet( items, s, item, i );
        if (done) break;
    }
}

static void _process_que_item(T _this_, item_ *item ) {

    Table_T items = _this_->items;

    if (item == NULL) {
        return;
    }

    switch (item->type) {
    
    case REG:
        OE_TLOG(0, "Oed_Dispatcher _process_que_item REG\n"); //i18n
        int rc = _add_template(_this_, item);
        //schedule timeout
        _watch_for_timeout(item, _this_->net_obj);

        break;

    case UNREG:
        _remove_item(items, item, false);
        break;

    case PROCESS:
        OE_TLOG(0, "Oed_Dispatcher _process_que_item PROCESS\n"); //i18n
        //run match tests
        _process(items, item);
        if (item->free_alloc) {
            _free_object(item->object);
        }
        Mem_free(item, __FILE__, __LINE__);
        break;

    case EXECUTE:
        //run now
        if (item->match_handler) {
            _process_fun(item->match_handler, item->args);
        }
        Mem_free(item, __FILE__, __LINE__);
        break;

    case HEARTBEAT:
        timeout_del(item->timer_event);
        Mem_free(item->timer_event, __FILE__, __LINE__);
        Mem_free(item, __FILE__, __LINE__);
        _schedule_heartbeat(_this_);
        break;

    case DIAG:
        Mem_free(item, __FILE__, __LINE__);
        _print_diag(_this_);
        break;

    case TQUIT:
        Mem_free(item, __FILE__, __LINE__);
        event_base_loopexit(_this_->net_obj, 0);
        _free_items(items);
        break;

    default:
        assert(0); //should never get here
        break;
    }
}

int (Oed_Dispatcher_stats)(T _this_) {

    assert(_this_);

    item_ *item;
    item = Mem_calloc(1, sizeof *item, __FILE__, __LINE__);
    item->dispatcher = _this_;
    item->type = DIAG;
    oe_id sid = _gen_id(_this_);
    item->sid = sid;

    _schedule_item(_this_, item);

    return 0;
}

#undef T

