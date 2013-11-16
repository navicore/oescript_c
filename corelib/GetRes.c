/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "GetRes.h"
#include "oec_values.h"

#define T GetRes

#define	TUPLE        0x0000001
#define	DESTROYED    0x0000002
#define	RET_ATTRS    0x0000004
#define	RET_BYTES    0x0000008

T GetRes_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    return _this_;
}
void GetRes_set_nresults(T _this_, size_t nr) {
    _this_->nresults = nr;
}
size_t GetRes_get_nresults(T _this_) {
    return _this_->nresults;
}
DataObject GetRes_get_data(T _this_) {
    if (!_this_->data) {
        _this_->data = DataObject_new(_this_->arena);
    }
    return _this_->data;
}
void GetRes_set_data(T _this_, DataObject values) {
    _this_->data = values;
}

void GetRes_set_destroyed(T _this_) {
    assert(_this_);
    OEC_FLAG_SET(_this_->flags, DESTROYED);
}
bool GetRes_is_destroyed(T _this_) {
    assert(_this_);
    return OEC_FLAG_ISSET(_this_->flags, DESTROYED);
}

void GetRes_set_tuple(T _this_) {
    assert(_this_);
    OEC_FLAG_SET(_this_->flags, TUPLE);
}
bool GetRes_is_tuple(T _this_) {
    assert(_this_);
    return OEC_FLAG_ISSET(_this_->flags, TUPLE);
}

void GetRes_set_return_bytes(T _this_) {
    assert(_this_);
    OEC_FLAG_SET(_this_->flags, RET_BYTES);
}
bool GetRes_return_bytes(T _this_) {
    assert(_this_);
    return OEC_FLAG_ISSET(_this_->flags, RET_BYTES);
}

void GetRes_set_return_attrs(T _this_) {
    assert(_this_);
    OEC_FLAG_SET(_this_->flags, RET_ATTRS);
}
bool GetRes_return_attrs(T _this_) {
    assert(_this_);
    return OEC_FLAG_ISSET(_this_->flags, RET_ATTRS);
}

#undef T

