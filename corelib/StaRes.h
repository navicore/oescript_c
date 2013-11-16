/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef STARES_H
#define STARES_H

#include <stddef.h>
#include "oec_values.h"
#include "arena.h"
#define T StaRes
struct stares_T {
    //oe_time expiretime;
    int _dummy; //a stares return data is just the tid in the msg envelope
};

typedef struct stares_T *T;

extern T StaRes_new(Arena_T arena);

//RES STA corid:number txn_id:number
/*
extern void    StaRes_set_expiretime(T, oe_time expiretime);
extern oe_time StaRes_get_expiretime(T);
*/

#undef T
#endif

#ifdef __cplusplus
}
#endif 

