/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "PutCmd.h"
#include "Oec_AList.h"
#include "oec_values.h"
#include <assert.h>

#define T PutCmd

T PutCmd_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    return _this_;
}

void PutCmd_set_tuple(T _this_) {
    assert(_this_);
    _this_->is_tuple = true;
}
bool PutCmd_is_tuple(T _this_) {
    assert(_this_);
    return _this_->is_tuple;
}
void PutCmd_set_dur(T _this_, oe_dur dur) {
    assert(_this_);
    _this_->duration = dur;
}
oe_dur PutCmd_get_dur(T _this_) {
    assert(_this_);
    return _this_->duration;
}
DataObject PutCmd_get_data(T _this_) { 
    assert(_this_);
    if (_this_->data == NULL) _this_->data = DataObject_new(_this_->arena);
    return _this_->data;
}

void PutCmd_set_data(T _this_, DataObject data) { 
    assert(_this_);
    _this_->data = data;
}

#undef T

