/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEKERR_H
#define OEKERR_H

#include <stddef.h>
#include "oec_values.h"
#include "arena.h"
#define T OekErr
struct oekerr_T {
    oe_id     type;
    oe_scalar emessage;
};
typedef struct oekerr_T *T;

extern T OekErr_new(Arena_T);
//ERR corid:number type:string message:string
extern void   OekErr_set_type(T, oe_id sid);
extern oe_id  OekErr_get_type(T);
extern void   OekErr_set_message(T, oe_scalar message);
extern oe_scalar OekErr_get_message(T);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

