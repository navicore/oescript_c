/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include <pthread.h>
#include "arena.h"
#include "mem.h"
#include "oescript_core_resources.h"
#include "oec_log.h"
#include "oed_dispatcher_common.h"

#define ThreadData_T pthread_key_t
#define ThreadData_set(key, value) pthread_setspecific((key), (value))
#define ThreadData_get(key) pthread_getspecific((key))
#define ThreadData_create(key) pthread_key_create(&(key), NULL);
ThreadData_T arena_td;

static pthread_once_t arena_once_control = PTHREAD_ONCE_INIT;
static void init_arena(void) {ThreadData_create(arena_td);}
#define init_arena_once() pthread_once(&(arena_once_control), init_arena)

//arena per thread -  //the arena gets cleared between tasks.  if your
// task is not short lived, consider calling free once in a while
Arena_T Oed_get_arena(void) {
	init_arena_once();
    Arena_T arena = ThreadData_get(arena_td);
    if (!arena) {
        arena = Arena_new();
        ThreadData_set(arena_td, arena);
    }
	return arena;
}

static void _handle_mem_error() {
    OE_ERR(999, OED_STR_MEM_ALLOC_ERROR);
}

void _process_fun(user_callback *fun, void *funarg) {
    TRY
        fun(funarg);
        Arena_free(Oed_get_arena()); //reset/free this thread's arena between tasks
    EXCEPT(Arena_Failed)
    _handle_mem_error();
    EXCEPT(Mem_Failed)
    _handle_mem_error();
    END_TRY;
}

