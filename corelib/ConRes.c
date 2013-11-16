/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "ConRes.h"
#include "oec_values.h"
#include <assert.h>

#define T ConRes

T ConRes_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    return _this_;
}
void ConRes_set_version_maj(T _this_, size_t ma) {
    _this_->server_version.major = ma;
}
size_t ConRes_get_version_maj(T _this_) {
    return _this_->server_version.major;
}
void ConRes_set_version_min(T _this_, size_t mi) {
    _this_->server_version.minor = mi;
}
size_t ConRes_get_version_min(T _this_) {
    return _this_->server_version.minor;
}
void ConRes_set_version_upt(T _this_, size_t up) {
    _this_->server_version.update = up;
}
size_t ConRes_get_version_upt(T _this_) {
    return _this_->server_version.update;
}
void ConRes_set_extentname(T _this_, oe_scalar extentname) {
    _this_->extentname = extentname;
}
oe_scalar ConRes_get_extentname(T _this_) {
    return _this_->extentname;
}
void ConRes_set_sid(T _this_, oe_id sid) {
    assert(_this_);
    _this_->session_id = sid;
}
oe_id ConRes_get_sid(T _this_) {
    assert(_this_);
    //assert(_this_->session_id);
    return _this_->session_id;
}

#undef T

