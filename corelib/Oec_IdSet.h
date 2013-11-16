/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef ID_SET_INCLUDED
#define ID_SET_INCLUDED

#include "oec_values.h"
#include "set.h"

#define T Oec_IdSet
struct idset_T {
    Set_T members;
};

typedef struct idset_T *T;

extern T       Oec_IdSet_new (int hint);

extern void    Oec_IdSet_free(T *idset);

extern int     Oec_IdSet_member(T idset, oe_id mid);

extern void    Oec_IdSet_put   (T idset, oe_id mid);

extern oe_id   Oec_IdSet_remove(T idset, oe_id mid);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

