/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus

extern "C" {
#endif 

#ifndef BASE_LOGIN_MODULE_H
#define BASE_LOGIN_MODULE_H

#include "assert.h"
#include "BaseLoginModule.h"
#include "LoginModule.h"
#include "mem.h"
#include "Oec_AList.h"
#include "oec_log.h"
#include "oec_values.h"
#include "OekMsg.h"
#include "Privilege.h"
#include "SKeyTable.h"
#include "stdbool.h"
#include "Subject.h"
#include "table.h"

#define T BaseLoginModule

struct base_loginmod_T {

    Arena_T         arena;
    ArenaTable_T    extents;
};

typedef struct base_loginmod_T *T;

#define MAX_GROUPS 10

//api
extern T BaseLoginModule_new(Arena_T);

extern void BaseLoginModule_assign_privs( T, Subject, oe_scalar*);

extern int BaseLoginModule_add_privilege( T , oe_scalar, oe_scalar, OekMsg);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

