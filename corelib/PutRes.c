/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "PutRes.h"
#include "oec_values.h"

#define T PutRes

T PutRes_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    return _this_;
}
/*
void    PutRes_set_expiretime(T _this_, oe_time time) {
    _this_->expiretime = time;
}
oe_time PutRes_get_expiretime(T _this_) {
    return _this_->expiretime;
}
*/
void    PutRes_set_lease_id(T _this_, oe_id lid) {
    _this_->lease_id = lid;
}
oe_id   PutRes_get_lease_id(T _this_) {
    return _this_->lease_id;
}

#undef T

