/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "GetCmd.h"
#include "arena.h"
#include "oec_values.h"

#define T GetCmd

#define	OE_GET_CMD_TUPLE        0x0000001
#define	OE_GET_CMD_IFEXISTS     0x0000002
#define	OE_GET_CMD_DESTROY      0x0000004
#define	OE_GET_CMD_JOIN         0x0000008
#define	OE_GET_CMD_RET_ATTRS    0x0000010
#define	OE_GET_CMD_RET_BYTES    0x0000020

T GetCmd_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    return _this_;
}

static DataObjectList _get_value_lists(T _this_) {
    if (_this_->value_lists == NULL) {
        _this_->value_lists = DataObjectList_new(_this_->arena);
    }
    return _this_->value_lists;
}

void GetCmd_set_timeout(T _this_, oe_dur timeout) {
    _this_->timeout = timeout;
}

oe_dur GetCmd_get_timeout(T _this_) {
    return _this_->timeout;
}

void GetCmd_set_join(T _this_) {
    //assert(!_this_->max_responses);
    _this_->max_responses = 0;
    OEC_FLAG_SET(_this_->flags, OE_GET_CMD_JOIN);
}

static bool _join_is_set(T _this_) {
    bool ret = OEC_FLAG_ISSET(_this_->flags, OE_GET_CMD_JOIN);
    return ret;
}
bool GetCmd_get_join(T _this_) {
    return _join_is_set(_this_) &&
        DataObjectList_length(_get_value_lists(_this_)) > 1;
}

void GetCmd_set_max_responses(T _this_, size_t maxr) {
    assert(maxr == 0 ? true : !_join_is_set(_this_));
    _this_->max_responses = maxr;
}

size_t GetCmd_get_max_responses(T _this_) {
    return _this_->max_responses;
}

void GetCmd_set_ifexists_option(T _this_) {
    OEC_FLAG_SET(_this_->flags, OE_GET_CMD_IFEXISTS);
}

bool GetCmd_ifexists(T _this_) {
    return OEC_FLAG_ISSET(_this_->flags, OE_GET_CMD_IFEXISTS);
}

void GetCmd_set_remove_option(T _this_) {
    OEC_FLAG_SET(_this_->flags, OE_GET_CMD_DESTROY);
}

bool GetCmd_remove(T _this_) {
    return OEC_FLAG_ISSET(_this_->flags, OE_GET_CMD_DESTROY);
}

void GetCmd_set_tuple(T _this_) {
    OEC_FLAG_SET(_this_->flags, OE_GET_CMD_TUPLE);
}

bool GetCmd_is_tuple(T _this_) {
    return OEC_FLAG_ISSET(_this_->flags, OE_GET_CMD_TUPLE);
}

void GetCmd_set_return_bytes(T _this_, bool option) {
    if (option) {
        OEC_FLAG_SET(_this_->flags, OE_GET_CMD_RET_BYTES);
    } else {
        OEC_FLAG_CLEAR(_this_->flags, OE_GET_CMD_RET_BYTES);
    }
}

bool GetCmd_return_bytes(T _this_) {
    return OEC_FLAG_ISSET(_this_->flags, OE_GET_CMD_RET_BYTES);
}

void GetCmd_set_return_attrs(T _this_, bool option) {
    if (option) {
        OEC_FLAG_SET(_this_->flags, OE_GET_CMD_RET_ATTRS);
    } else {
        OEC_FLAG_CLEAR(_this_->flags, OE_GET_CMD_RET_ATTRS);
    }
}

bool GetCmd_return_attrs(T _this_) {
    return OEC_FLAG_ISSET(_this_->flags, OE_GET_CMD_RET_ATTRS);
}

DataObjectList GetCmd_get_data_objects(T _this_) {
    return _get_value_lists(_this_);
}

DataObject GetCmd_new_data_object(T _this_ ) {
    return DataObjectList_new_data_object(_get_value_lists(_this_));
}

void GetCmd_set_data_objects(T _this_, DataObjectList value_lists) {
    assert(_this_);
    assert(!_this_->value_lists);
    assert(value_lists);
    _this_->value_lists = value_lists;
}

#undef T

