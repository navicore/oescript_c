/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "StaCmd.h"
#include "oec_values.h"

#define T StaCmd

T StaCmd_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    return _this_;
}
//ST _this_A corid:number duration:number
void   StaCmd_set_dur(T _this_, oe_dur dur) {
    _this_->duration = dur;
}
oe_dur StaCmd_get_dur(T _this_) {
    return _this_->duration;
}

#undef T

