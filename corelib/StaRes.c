/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "StaRes.h"
#include "oec_values.h"

#define T StaRes

T StaRes_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    return _this_;
}
/*
void StaRes_set_expiretime(T _this_, oe_time time) {
    _this_->expiretime = time;
}
oe_time StaRes_get_expiretime(T _this_) {
    return _this_->expiretime;
}
*/

#undef T

