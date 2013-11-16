/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef CONRES_H
#define CONRES_H

#include <stddef.h>
#include "arena.h"
#include "oec_values.h"

#define T ConRes
struct conres_T {
    struct {
        size_t major; 
        size_t minor; 
        size_t update; 
    } server_version;

    oe_scalar extentname;
    oe_id session_id;
};
typedef struct conres_T *T;

extern T ConRes_new(Arena_T arena);

//RES CON version:string extentname:string session_id:string
extern void   ConRes_set_version_maj(T, size_t ma);
extern size_t ConRes_get_version_maj(T);
extern void   ConRes_set_version_min(T, size_t mi);
extern size_t ConRes_get_version_min(T);
extern void   ConRes_set_version_upt(T, size_t up);
extern size_t ConRes_get_version_upt(T);
extern void   ConRes_set_extentname(T, oe_scalar extentname);
extern oe_scalar ConRes_get_extentname(T);
extern void   ConRes_set_sid(T, oe_id sid);
extern oe_id  ConRes_get_sid(T);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

