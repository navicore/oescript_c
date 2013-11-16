/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "BaseLoginModule.h"
#include "BuiltInLoginModule.h"
#include "assert.h"
#include "Subject.h"
#include "oec_values.h"
#include "arenatable.h"
#include "SKeyTable.h"
#include "Oec_AList.h"
#include "oec_log.h"
#include <string.h>

#define T BuiltInLoginModule

typedef struct userinfo userinfo;
struct userinfo {
    oe_scalar username;
    oe_scalar pwd;
    oe_scalar extentname;
    oe_scalar grpname[MAX_GROUPS];
};

static void _login( T, Subject);
static void _free( T* );

T BuiltInLoginModule_new() {

    T _this_;
    Arena_T arena = Arena_new();
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    _this_->users = SKeyTable_arena_new(arena, 10000);
    _this_->basemod = BaseLoginModule_new(arena);

    return _this_;
}

Arena_T BuiltInLoginModule_get_arena(T _this_) {
    return _this_->arena;
}

LoginModule BuiltInLoginModule_new_module(T _this_) {

    return LoginModule_new(_login, _free, _this_);
}

static void _free( T* _this_p ) {
    T _this_ = *_this_p;
    Arena_dispose(&_this_->arena);
}

static void _login( T _this_, Subject s) {
    assert(_this_);
    assert(s);
    oe_scalar username = Subject_get_username(s);
    if (username == NULL)  return;
    userinfo *uinfo = ArenaTable_get(_this_->users, username);
    if (uinfo == NULL)  return;

    if (Subject_is_authenticated(s)) {
        return; //some other module has authenticated this user, it is up to that
                // module to assign privs
    }
    oe_scalar pwd = uinfo->pwd;
    oe_scalar trymepwd = Subject_get_pwd(s);
    if (trymepwd == NULL) return;
    if (strncmp(pwd, trymepwd, strlen(pwd)) != 0) {
        OE_DLOG( NULL, "BuiltInLoginModule pwd does not match for user %s\n", username); //i18n
        return;
    } else {
        OE_DLOG( NULL, "BuiltInLoginModule user %s is authenticated\n", username); //i18n
        Subject_status_set(s, SUBJECT_AUTHENTICATED);
    }
    // can not set this until login time because you don't know what extent to associate
    // the privs with
    BaseLoginModule_assign_privs(_this_->basemod, s, &uinfo->grpname);
}

int BuiltInLoginModule_add_account( T _this_ , 
                                    oe_scalar grouplst,
                                    oe_scalar username, 
                                    oe_scalar pwd) {

    Arena_T arena = _this_->arena;
    oe_scalar unamecpy = oec_cpy_str(arena, username);
    oe_scalar pwdcpy = oec_cpy_str(arena, pwd);
    oe_scalar grpnamecpy = oec_cpy_str(arena, grouplst);
    userinfo *uinfo = Arena_calloc(arena, 1, sizeof (userinfo), __FILE__, __LINE__);
    uinfo->username = unamecpy;
    uinfo->pwd = pwdcpy;
    char *save_ptr;
    char *token, *cp;
    cp = oec_cpy_str(arena, grouplst);
    const char delimiters[] = " .,;:!-";
    int i;
    for (i = 0; i < MAX_GROUPS; i++) {
        token = strtok_r(cp, delimiters, &save_ptr);
        cp = NULL;
        uinfo->grpname[i] = token;
        if (token == NULL) break;
    }

    ArenaTable_put(_this_->users, unamecpy, uinfo);
    return 0;
}

int BuiltInLoginModule_add_privilege( T _this_, 
                                      oe_scalar extentname,
                                      oe_scalar groupname,
                                      OekMsg mtemplate) {
    return BaseLoginModule_add_privilege(_this_->basemod, extentname, groupname, mtemplate);
}

#undef T

