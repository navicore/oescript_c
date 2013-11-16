/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "UptRes.h"
#include "oec_values.h"

#define T UptRes

T UptRes_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    return _this_;
}
/*
void    UptRes_set_expiretime(T _this_, oe_time time) {
    _this_->expiretime = time;
}
oe_time UptRes_get_expiretime(T _this_) {
    return _this_->expiretime;
}
*/
void    UptRes_set_status(T _this_, char status) {
    _this_->status = status;
}
char    UptRes_get_status(T _this_) {
    if (!_this_->status) {
        return OETXN_ACTIVE;
    }
    return _this_->status;
}

#undef T

