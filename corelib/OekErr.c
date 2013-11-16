/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "OekErr.h"
#include "oec_values.h"

#define T OekErr

T OekErr_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    return _this_;
}

void OekErr_set_type(T _this_, oe_id type) {
    _this_->type = type;
}
oe_id OekErr_get_type(T _this_) {
    return _this_->type;
}

void OekErr_set_message(T _this_, oe_scalar emessage) {
    _this_->emessage = emessage;
}
oe_scalar OekErr_get_message(T _this_) {
    return _this_->emessage;
}

#undef T

