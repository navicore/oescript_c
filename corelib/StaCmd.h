/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef STACMD_H
#define STACMD_H

#include <stddef.h>
#include "oec_values.h"
#include <stdbool.h>
#define T StaCmd
#include "arena.h"
struct stacmd_T {
    oe_dur   duration;
    bool     rollback_on_disconnect;
};
typedef struct stacmd_T *T;

extern T StaCmd_new(Arena_T arena);

//STA corid:number duration:number

extern void   StaCmd_set_dur(T, oe_dur dur);
extern oe_dur StaCmd_get_dur(T);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

