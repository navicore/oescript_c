/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "ConCmd.h"
#include "oec_values.h"

#define T ConCmd

#define	OE_ROLLBACK_ON_DISCONNECT        0x0000001

//CON version:string extentname:string username:string password:string

T ConCmd_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    return _this_;
}
void ConCmd_set_rollback_on_disconnect(T _this_) {
    OEC_FLAG_SET(_this_->flags, OE_ROLLBACK_ON_DISCONNECT);
}

bool ConCmd_rollback_on_disconnect(T _this_) {
    return OEC_FLAG_ISSET(_this_->flags, OE_ROLLBACK_ON_DISCONNECT);
}

void ConCmd_set_version_maj(T _this_, size_t ma) {
    _this_->client_version.major = ma;
}
size_t ConCmd_get_version_maj(T _this_) {
    return _this_->client_version.major;
}
void ConCmd_set_version_min(T _this_, size_t mi) {
    _this_->client_version.minor = mi;
}
size_t ConCmd_get_version_min(T _this_) {
    return _this_->client_version.minor;
}
void ConCmd_set_version_upt(T _this_, size_t up) {
    _this_->client_version.update = up;
}
size_t ConCmd_get_version_upt(T _this_) {
    return _this_->client_version.update;
}
void ConCmd_set_extentname(T _this_, oe_scalar extentname) {
    _this_->extentname = extentname;
}
oe_scalar ConCmd_get_extentname(T _this_) {
    return _this_->extentname;
}
void ConCmd_set_username(T _this_, oe_scalar username) {
    _this_->username = username;
}
oe_scalar ConCmd_get_username(T _this_) {
    return _this_->username;
}
void ConCmd_set_pwd(T _this_, oe_scalar pwd) {
    _this_->password = pwd;
}
oe_scalar ConCmd_get_pwd(T _this_) {
    return _this_->password;
}

#undef T

