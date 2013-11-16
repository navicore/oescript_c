/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef UPLCMD_H
#define UPLCMD_H

#include <stddef.h>
#include "oec_values.h"
#define T UplCmd
#include "arena.h"
struct uplcmd_T {
    oe_id   lease_id;
    oe_dur  duration;
};
typedef struct uplcmd_T *T;

extern T UplCmd_new(Arena_T arena);

//UPL corid:number lease_id:number duration:number
extern void   UplCmd_set_lid(T, oe_id lid);
extern oe_id  UplCmd_get_lid(T);
extern void   UplCmd_set_dur(T, oe_dur dur);
extern oe_dur UplCmd_get_dur(T);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

