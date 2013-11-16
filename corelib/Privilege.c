/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "Privilege.h"
#include "arena.h"
#include "OekMsg.h"
#include <assert.h>

#define T Privilege

T Privilege_new(Arena_T arena, PRIVILEGE_allow_func *allow_func, void *impl) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    _this_->impl = impl;
    _this_->allow_func = allow_func;
    return _this_;
}

Arena_T Privilege_get_arena(T _this_ ) {
    assert(_this_->arena);
    return _this_->arena;
}

AUTH Privilege_allow(T _this_, OekMsg msg) {
    assert(_this_->allow_func);
    assert(_this_->impl);
    assert(msg);
    return _this_->allow_func(_this_->impl, msg);
}

#undef T

