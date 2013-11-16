/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "Iterator.h"
#include "mem.h"
#include <stdbool.h>
#include "assert.h"

#define T Iterator

static int DONE = -1;

T Iterator_new(ITERATOR_next_call *next,
               ITERATOR_hasMore_call *hasMore,
               ITERATOR_free_call *ifree,
               ITERATOR_state_constructor *iter_state_new,
               void *iter_state_new_arg, bool with_arena) {
    T _this_;
    _this_ = Mem_alloc(sizeof *_this_, __FILE__, __LINE__);
    _this_->next = next;
    _this_->hasMore = hasMore;
    _this_->ifree = ifree;
    _this_->iter_state_new = iter_state_new;
    _this_->iter_state_new_arg = iter_state_new_arg;
    _this_->iter_state = NULL;
    _this_->with_arena = with_arena;
    return _this_;
}

void _state_init(T _this_) {
    if (_this_->iter_state == NULL) {
        _this_->iter_state = _this_->iter_state_new(_this_->iter_state_new_arg);
    }
}

bool Iterator_hasMore(T _this_) {
    _state_init(_this_); //lazy
    return _this_->hasMore(_this_->iter_state);
}

void *Iterator_next(T _this_) {
    _state_init(_this_); //lazy
    return _this_->next(_this_->iter_state);
}

void Iterator_free(T *_this_p_) {
    assert(_this_p_ && *_this_p_);
    T _this_ = *_this_p_;
    if (!_this_->with_arena && _this_->iter_state_new_arg) {
        _this_->ifree(&_this_->iter_state);
    }
    Mem_free(_this_, __FILE__, __LINE__);
}

#undef T

