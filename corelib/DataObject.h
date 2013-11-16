/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef STRINGLIST_H
#define STRINGLIST_H

#include "arena.h"
#include <stddef.h>
#include "oec_values.h"
#include "Iterator.h"
#include "Oec_AList.h"

#define T DataObject
struct dataobject_T {
    Arena_T   arena;
    Oec_AList strings;
    size_t    nbytes;
    char     *bytes;
    void    **array_cache;
    bool    always_cpy;
};
typedef struct dataobject_T *T;

extern T DataObject_new(Arena_T);

extern size_t DataObject_get_nattrs(T);
extern void DataObject_add_attr(T, oe_scalar);

extern void DataObject_set_nbytes(T, size_t);
extern size_t DataObject_get_nbytes(T);
extern void DataObject_set_bytes(T, char *);
extern char *DataObject_get_bytes(T);

extern oe_scalar *DataObject_toArray(T);

extern Iterator DataObject_iterator(T, bool);
extern void DataObject_set_always_copy(T, bool);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

