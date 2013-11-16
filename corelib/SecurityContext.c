/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "SecurityContext.h"
#include "Oec_AList.h"
#include "oec_values.h"
#include "arena.h"
#include "OekMsg.h"
#include "oec_log.h"

#define T SecurityContext

T SecurityContext_new() {
    T _this_;
    Arena_T arena = Arena_new();
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    return _this_;
}

void SecurityContext_login( T _this_, Subject s) {
    ArrayIterator iter = Oec_AList_iterator(_this_->modules);
    //any one of the modules can mark the subject authenticated but
    // once a module is athenticated any of the successive modules
    // can add privs without checking creds.
    while (ArrayIterator_hasMore(iter)) {
        LoginModule m = ArrayIterator_next(iter);
        LoginModule_login(m, s);
    }
    ArrayIterator_free(&iter);
}

AUTH SecurityContext_authorize( T _this_, Subject s, OekMsg msg ) {
    OE_DLOG(0, "SecurityContext_authorize test privs for user: %s on ext: %s\n", Subject_get_username(s), Subject_get_extentname(s));
    AUTH auth = ALLOW;
    ArrayIterator iter = Subject_get_privileges(s);
    while (ArrayIterator_hasMore(iter)) {
        OE_DLOG(0, "... testing priv for user: %s on ext: %s\n", Subject_get_username(s), Subject_get_extentname(s));
        Privilege p = ArrayIterator_next(iter);
        auth = Privilege_allow(p, msg);
        if (auth == DONTCARE) continue;
        break;
    }
    ArrayIterator_free(&iter);
    if (auth == DONTCARE) auth = DENY; //if nobody cares, deny by default
    return auth;
}

void SecurityContext_add_module(T _this_, LoginModule module) {
    _this_->modules = Oec_AList_append(
        _this_->modules, Oec_AList_list(_this_->arena, module, NULL) );
}
 
void SecurityContext_free( T* _this_p ) {
    T _this_ = *_this_p;
    Arena_free(_this_->arena);
}

#undef T

