/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef GETCMD_H
#define GETCMD_H

#include "arena.h"
#include "DataObject.h"
#include "DataObjectList.h"
#include <stddef.h>
#include "oec_values.h"
#include <stdbool.h>
#define T GetCmd
struct getcmd_T {
    oe_dur    timeout;
    size_t    max_responses;
    oe_flags  flags;
    Arena_T   arena;
    DataObjectList value_lists;
};

typedef struct getcmd_T *T;

T GetCmd_new(Arena_T arena);

//GET corid:number txn_id:number timeout:number max:number ifexists:boolean destroy:boolean returnkeys.boolean key*:string
extern bool   GetCmd_ifexists(T);
extern bool   GetCmd_get_join(T);
extern bool   GetCmd_remove(T);
extern bool   GetCmd_return_attrs(T);
extern bool   GetCmd_return_bytes(T);
extern bool   GetCmd_is_tuple(T);
extern DataObject GetCmd_new_data_object( T );
extern DataObjectList GetCmd_get_data_objects(T);
extern oe_dur GetCmd_get_timeout(T);
extern size_t GetCmd_get_max_responses(T);

extern void   GetCmd_set_data_objects(T, DataObjectList);
extern void   GetCmd_set_ifexists_option(T);
extern void   GetCmd_set_join(T);
extern void   GetCmd_set_max_responses(T, size_t maxr);
extern void   GetCmd_set_remove_option(T);
extern void   GetCmd_set_return_attrs(T, bool);
extern void   GetCmd_set_return_bytes(T, bool);
extern void   GetCmd_set_timeout(T, oe_dur timeout);
extern void   GetCmd_set_tuple(T);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

