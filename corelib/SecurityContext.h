/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OE_SECURITY_CONTEXT_H
#define OE_SECURITY_CONTEXT_H

#include "Subject.h"
#include "Privilege.h"
#include "LoginModule.h"
#include "OekMsg.h"

#define T SecurityContext
struct seccontext_T {

    Oec_AList modules;
    Arena_T arena;
};

typedef struct seccontext_T *T;

extern T SecurityContext_new();

//
// each LoginModule will verify the creds and assign
// privileges.
//  - if you are assigned any privileges by any module, you
//    are considered signed in and will be allowed to try to
//    do stuff.
//
extern void SecurityContext_login( T, Subject);

//
//  when you try to do stuff, each privilege can say 'yes',
//    'no', and 'no opinion'.
//      - the first priv that says 'yes' means AUTH
//      - the first priv that says 'no' means DENY
//      - if the loop completes and all modules say
//        'no opinion' then DENY
//
extern AUTH SecurityContext_authorize( T, Subject, OekMsg);

extern void SecurityContext_free( T* );

extern void SecurityContext_add_module( T, LoginModule );

#undef T

#endif

#ifdef __cplusplus
}
#endif 

