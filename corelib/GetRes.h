/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef GETRES_H
#define GETRES_H

#include "DataObject.h"
#include <stddef.h>
#include "arena.h"
#include "oec_values.h"
#define T GetRes
struct getres_T {
    Arena_T arena;
    size_t  nresults;
    oe_flags  flags;
    DataObject data;
};

typedef struct getres_T *T;

T GetRes_new(Arena_T arena);

//RES GET corid:number result_set_size:number bytes_length:number key*:string	
extern void    GetRes_set_nresults(T, size_t nr);
extern size_t  GetRes_get_nresults(T);

extern DataObject GetRes_get_data(T);
extern void    GetRes_set_data(T, DataObject);

extern bool   GetRes_is_tuple(T);
extern void   GetRes_set_tuple(T);

extern bool   GetRes_is_destroyed(T);
extern void   GetRes_set_destroyed(T);

extern bool   GetRes_return_attrs(T);
extern void   GetRes_set_return_attrs(T);

extern bool   GetRes_return_bytes(T);
extern void   GetRes_set_return_bytes(T);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

