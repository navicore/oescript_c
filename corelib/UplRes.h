/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef UPLRES_H
#define UPLRES_H

#include <stddef.h>
#include "oec_values.h"
#define T UplRes
#include "arena.h"
struct uplres_T {
    oe_time expiretime;
};
typedef struct uplres_T *T;

extern T UplRes_new(Arena_T arena);

//RES UPL corid:number expire_time:number
extern void    UplRes_set_expiretime(T, oe_time time);
extern oe_time UplRes_get_expiretime(T);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

