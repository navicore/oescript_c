/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef UPTRES_H
#define UPTRES_H

#include <stddef.h>
#include "oec_values.h"
#define T UptRes
#include "arena.h"
struct uptres_T {
//    oe_time expiretime;
    char    status;
};
typedef struct uptres_T *T;

#define OETXN_ACTIVE 'A'
#define OETXN_ROLLEDBACK 'R'
#define OETXN_COMMITTED 'C'

extern T UptRes_new(Arena_T arena);

//RES UPT corid:number status:char[C|R|A]
/*
extern void    UptRes_set_expiretime(T, oe_time time);

extern oe_time UptRes_get_expiretime(T);
*/

extern void    UptRes_set_status(T, char status);

extern char    UptRes_get_status(T);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

