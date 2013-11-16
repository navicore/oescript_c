/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef CONCMD_H
#define CONCMD_H

#include "arena.h"
#include "oec_values.h"
#include <stddef.h>
#include <stdbool.h>
#define T ConCmd
struct concmd_T {
    struct {
        size_t major; 
        size_t minor; 
        size_t update; 
    } client_version;

    oe_scalar extentname;
    oe_scalar username;
    oe_scalar password;
    oe_flags  flags;
};
typedef struct concmd_T *T;

extern T ConCmd_new(Arena_T arena);

extern void    ConCmd_set_version_maj(T, size_t ma);
extern size_t  ConCmd_get_version_maj(T);
extern void    ConCmd_set_version_min(T, size_t mi);
extern size_t  ConCmd_get_version_min(T);
extern void    ConCmd_set_version_upt(T, size_t up);
extern size_t  ConCmd_get_version_upt(T);
extern void    ConCmd_set_extentname(T, oe_scalar extentname);
extern oe_scalar ConCmd_get_extentname(T);
extern void    ConCmd_set_username(T, oe_scalar username);
extern oe_scalar ConCmd_get_username(T);
extern void    ConCmd_set_pwd(T, oe_scalar pwd);
extern oe_scalar ConCmd_get_pwd(T);
extern void ConCmd_set_rollback_on_disconnect(T);
extern bool ConCmd_rollback_on_disconnect(T);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

