/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "UplCmd.h"
#include "oec_values.h"

#define T UplCmd

T UplCmd_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    return _this_;
}
void UplCmd_set_lid(T _this_, oe_id lid) {
    _this_->lease_id = lid;
}
oe_id UplCmd_get_lid(T _this_) {
    return _this_->lease_id;
}
void UplCmd_set_dur(T _this_, oe_dur dur) {
    _this_->duration = dur;
}
oe_dur UplCmd_get_dur(T _this_) {
    return _this_->duration;
}

#undef T

