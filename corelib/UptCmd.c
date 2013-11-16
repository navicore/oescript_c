/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "UptCmd.h"
#include "oec_values.h"

#define T UptCmd

T UptCmd_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    return _this_;
}
void UptCmd_set_dur(T _this_, oe_dur dur) {
    _this_->duration = dur;
}
oe_dur UptCmd_get_dur(T _this_) {
    return _this_->duration;
}
void UptCmd_set_status(T _this_, char status) {
    _this_->status = status;
}
char UptCmd_get_status(T _this_) {
    return _this_->status;
}
void UptCmd_set_enlisted(T _this_, int enlisted) {
    _this_->enlisted = enlisted;
}
int UptCmd_get_enlisted(T _this_) {
    return _this_->enlisted;
}


#undef T

