/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OE_BUILTIN_LOGIN_MODULE_H
#define OE_BUILTIN_LOGIN_MODULE_H

#include "BuiltInLoginModule.h"
#include "BaseLoginModule.h"
#include "Privilege.h"
#include "Subject.h"
#include "LoginModule.h"
#include "oec_values.h"
#include "OekMsg.h"

#define T BuiltInLoginModule
struct bi_login_mod_T {

    Arena_T         arena;
    ArenaTable_T    users;
    BaseLoginModule basemod;
};
typedef struct bi_login_mod_T *T;

//api
extern T BuiltInLoginModule_new();

extern int BuiltInLoginModule_add_account( T , oe_scalar,
                                           oe_scalar, oe_scalar);

extern int BuiltInLoginModule_add_privilege( T , oe_scalar, oe_scalar, OekMsg);

extern Arena_T BuiltInLoginModule_get_arena(T);

extern LoginModule BuiltInLoginModule_new_module(T _this_);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

