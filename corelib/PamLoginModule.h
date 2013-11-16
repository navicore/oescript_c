/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OE_PAM_LOGIN_MODULE_H
#define OE_PAM_LOGIN_MODULE_H

#include "PamLoginModule.h"
#include "Privilege.h"
#include "Subject.h"
#include "LoginModule.h"
#include "oec_values.h"
#include "OekMsg.h"

#define T PamLoginModule
typedef struct T *T;

//api
extern T PamLoginModule_new(oe_scalar ,oe_scalar);

extern void PamLoginModule_free( T* );

extern int PamLoginModule_add_privilege( T , oe_scalar, oe_scalar, OekMsg);

extern Arena_T PamLoginModule_get_arena(T);

extern LoginModule PamLoginModule_new_module(T _this_);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

