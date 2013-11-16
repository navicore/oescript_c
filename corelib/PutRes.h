/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef PUTRES_H
#define PUTRES_H

#include <stddef.h>
#include "arena.h"
#include "oec_values.h"
#define T PutRes
struct putres_T {
    //oe_time expiretime;
    oe_id   lease_id;
};
typedef struct putres_T *T;

T PutRes_new(Arena_T arena);

//RES PUT corid:number expiretime:number lease_id:number
//extern void    PutRes_set_expiretime(T, oe_time expiretime);
//extern oe_time PutRes_get_expiretime(T);
extern void    PutRes_set_lease_id(T, oe_id lid);
extern oe_id   PutRes_get_lease_id(T);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

