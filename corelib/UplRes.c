/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "UplRes.h"
#include "oec_values.h"

#define T UplRes

T UplRes_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    return _this_;
}
void    UplRes_set_expiretime(T _this_, oe_time time) {
    _this_->expiretime = time;
}
oe_time UplRes_get_expiretime(T _this_) {
    return _this_->expiretime;
}

#undef T

