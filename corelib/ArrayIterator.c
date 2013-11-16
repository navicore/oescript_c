/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "ArrayIterator.h"
#include "mem.h"
#include "assert.h"
#include <stdbool.h>

#define T ArrayIterator

static int DONE = -1;

T ArrayIterator_new(void **darray) {
    assert(darray);
    T _this_;
    _this_ = Mem_alloc(sizeof *_this_, __FILE__, __LINE__);
    _this_->darray = darray;
    _this_->pos =  (darray == NULL || darray[0] == NULL) ? DONE : 0;
    return _this_;
}

void *ArrayIterator_next(T _this_) {
    if (_this_->pos == DONE) return NULL;
    void *data = _this_->darray[_this_->pos++];
    if (_this_->darray[_this_->pos] == NULL) _this_->pos = DONE;
    return data;
}

bool ArrayIterator_hasMore(T _this_) {
    return _this_->pos != DONE;
}

void ArrayIterator_free(T *_this_p_) {
    assert(_this_p_ && *_this_p_);
    T _this_ = *_this_p_;
    Mem_free(_this_->darray, __FILE__, __LINE__);
    Mem_free(_this_, __FILE__, __LINE__);
}

#undef T

