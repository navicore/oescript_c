/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef PUTCMD_H
#define PUTCMD_H

#include <stddef.h>
#include <stdbool.h>
#include "arena.h"
#include "DataObject.h"
#include "oec_values.h"
#define T PutCmd
struct putcmd_T {
    oe_dur     duration;
    bool       is_tuple;
    Arena_T    arena;
    DataObject data;
};

typedef struct putcmd_T *T;

extern T PutCmd_new(Arena_T arena);

//PUT corid:number txn_id:number duration:number bytes_length:number key*:string
extern bool   PutCmd_is_tuple(T);
extern void   PutCmd_set_tuple(T);

extern void   PutCmd_set_dur(T, oe_dur);
extern oe_dur PutCmd_get_dur(T);

extern DataObject PutCmd_get_data(T _this_);
extern void   PutCmd_set_data(T, DataObject);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

