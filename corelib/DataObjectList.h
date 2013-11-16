/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef STRINGLISTLIST_H
#define STRINGLISTLIST_H

#include "arena.h"
#include <stddef.h>
#include "oec_values.h"
#include "DataObject.h"
#include "Oec_AList.h"
#include "Iterator.h"

#define T DataObjectList
struct dataobjectlist_T {
    Arena_T   arena;
    Oec_AList lists;
};
typedef struct dataobjectlist_T *T;

extern T DataObjectList_new(Arena_T);

extern size_t DataObjectList_length(T);
extern DataObject DataObjectList_new_data_object(T);
extern void DataObjectList_add(T, DataObject);
extern Iterator DataObjectList_iterator(T, bool);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

