/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef UPTCMD_H
#define UPTCMD_H

#include <stddef.h>
#include "oec_values.h"
#define T UptCmd
#include "arena.h"
struct uptcmd_T {
    oe_dur  duration;
    int     enlisted;
    char    status;
};
typedef struct uptcmd_T *T;

extern T UptCmd_new(Arena_T arena);

extern void   UptCmd_set_dur(T, oe_dur dur);
extern oe_dur UptCmd_get_dur(T);
extern void   UptCmd_set_status(T, char status);
extern char   UptCmd_get_status(T);
extern void   UptCmd_set_enlisted(T _this_, int enlisted);
extern int    UptCmd_get_enlisted(T _this_);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

