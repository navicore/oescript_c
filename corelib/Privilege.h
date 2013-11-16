/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OE_PRIVILEGE_H
#define OE_PRIVILEGE_H

#include "arena.h"
#include "OekMsg.h"

enum _auth {
    ALLOW, DENY, DONTCARE
};
typedef enum _auth AUTH ; 

typedef AUTH PRIVILEGE_allow_func(void *, OekMsg);

#define T Privilege
struct priv_T {

    Arena_T                 arena;
    void *                  impl;
    PRIVILEGE_allow_func    *allow_func;
};

typedef struct priv_T *T;

extern T Privilege_new(Arena_T, PRIVILEGE_allow_func *, void *);

extern Arena_T Privilege_get_arena(T);

extern AUTH Privilege_allow(T, OekMsg);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

