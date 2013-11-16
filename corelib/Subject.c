/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "Subject.h"
#include "Oec_AList.h"
#include "arena.h"
#include "Privilege.h"
#include "ArrayIterator.h"
#include "oec_values.h"

#define T Subject

T Subject_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    return _this_;
}

Arena_T Subject_get_arena(T _this_ ) {
    return _this_->arena;
}

bool Subject_has_news( T _this_) {
    return OEC_FLAG_ISSET(_this_->status, SUBJECT_HAS_NEWS);
}
bool Subject_is_authenticated( T _this_) {
    return OEC_FLAG_ISSET(_this_->status, SUBJECT_AUTHENTICATED);
}
bool Subject_status_is_set( T _this_, oe_flags flag ) {
    return OEC_FLAG_ISSET(_this_->status, flag);
}
void Subject_status_clear( T _this_, oe_flags flag ) {
    OEC_FLAG_CLEAR(_this_->status, flag);
}
void Subject_status_set( T _this_, oe_flags flag ) {
    OEC_FLAG_SET(_this_->status, flag);
}

void Subject_set_extentname(T _this_, oe_scalar extentname) {
    _this_->extentname = oec_cpy_str(_this_->arena, extentname);
}
oe_scalar Subject_get_extentname(T _this_) {
    return _this_->extentname;
}

void Subject_set_username(T _this_, oe_scalar username) {
    _this_->username = oec_cpy_str(_this_->arena, username);
}
oe_scalar Subject_get_username(T _this_) {
    return _this_->username;
}

void Subject_set_pwd(T _this_, oe_scalar pwd) {
    _this_->pwd = oec_cpy_str(_this_->arena, pwd);
}
oe_scalar Subject_get_pwd(T _this_) {
    return _this_->pwd;
}

ArrayIterator Subject_get_privileges(T _this_) {
    return Oec_AList_iterator(_this_->privileges);
}
void Subject_add_privilege(T _this_, Privilege priv) {
    _this_->privileges = Oec_AList_append(
        _this_->privileges, Oec_AList_list(_this_->arena, priv, NULL) );
}

ArrayIterator Subject_get_news(T _this_) {
    return Oec_AList_iterator(_this_->news);
}
void Subject_add_news(T _this_, char *txt) {
    _this_->news = Oec_AList_append(
        _this_->news, Oec_AList_list(_this_->arena, txt, NULL) );
}

#undef T

