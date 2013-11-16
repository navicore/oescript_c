/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef DISCMD_H
#define DISCMD_H

#include "arena.h"
#include "oec_values.h"

#define T DisCmd
struct discmd_T {
    oe_scalar msg;
};
typedef struct discmd_T *T;

extern T DisCmd_new(Arena_T arena);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

