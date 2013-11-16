/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEC_ITERATOR_H
#define OEC_ITERATOR_H

#include "arena.h"
#include <stdbool.h>

typedef void *ITERATOR_next_call(void *);
typedef bool ITERATOR_hasMore_call(void *);
typedef void ITERATOR_free_call(void *);
typedef void *ITERATOR_state_constructor(void *);

#define T Iterator
struct iterator_T {
    ITERATOR_next_call *next;
    ITERATOR_hasMore_call *hasMore;
    ITERATOR_free_call *ifree;
    void *iter_state;
    ITERATOR_state_constructor *iter_state_new;
    void *iter_state_new_arg;
    bool with_arena;
};
typedef struct iterator_T *T;

extern T Iterator_new(ITERATOR_next_call *,
                      ITERATOR_hasMore_call *,
                      ITERATOR_free_call *,
                      ITERATOR_state_constructor,
                      void *, bool);

extern void *Iterator_next(T);
extern bool Iterator_hasMore(T);
extern void Iterator_free(T*);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

