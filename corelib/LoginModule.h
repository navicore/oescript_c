/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OE_SECURITY_MODULE_H
#define OE_SECURITY_MODULE_H

#include "Privilege.h"
#include "Subject.h"

typedef AUTH LOGIN_MODULE_login(void *, Subject);
typedef AUTH LOGIN_MODULE_free(void *);

#define T LoginModule
struct loginmod_T {

    void *                  impl;
    LOGIN_MODULE_login      *login_func;
    LOGIN_MODULE_free       *free_func;
};

typedef struct loginmod_T *T;

/**
 * an implementation module must provide a login 
 * function and a ref to itself. 
 *  
 * the login function's job is to verifiy the creds, 
 * if so, mark the subject "logged in" and add 
 * privileges to the subject. see Subject docs. 
 * 
 * @author esweeney (6/10/2011)
 * 
 * @return T 
 */
extern T LoginModule_new(LOGIN_MODULE_login *,
                         LOGIN_MODULE_free *,
                         void *);

extern void LoginModule_login( T, Subject);

extern void LoginModule_free( T* );

#undef T

#endif

#ifdef __cplusplus
}
#endif 

