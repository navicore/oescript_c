/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OESTOREQUERY_H
#define OESTOREQUERY_H

#include "config.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "OeStoreHolder.h"
#include "OeStoreQuery.h"
#include "DataObjectList.h"
#include "oec_values.h"
#include "arena.h"

#define T OeStoreQuery
struct store_query_T {

    Arena_T arena;

    //query
    bool take;
    bool is_tuple;
    bool ifexists;
    oe_id owner_id;
    DataObjectList value_lists;
    size_t max_items_to_return;

    //results
    bool exists;
    Oec_AList results;
};

typedef struct store_query_T *T;

extern T OeStoreQuery_new(Arena_T, size_t);

//
//results
//
extern void OeStoreQuery_add_store_holder(T _this_,  OeStoreHolder holder);

extern OeStoreHolder OeStoreQuery_get_store_holder(T _this_, size_t idx);

extern void OeStoreQuery_reset_nitems_found(T _this_);

extern size_t OeStoreQuery_get_nitems_found(T _this_);

extern void OeStoreQuery_set_exists(T _this_,  bool exists);
extern bool OeStoreQuery_exists(T _this_);

//
//query
//

extern void OeStoreQuery_set_value_lists(T _this_,  DataObjectList value_lists);
extern DataObjectList OeStoreQuery_get_value_lists(T _this_);

extern void OeStoreQuery_set_owner(T _this_,  oe_id oid);
extern oe_id OeStoreQuery_get_owner_id(T _this_);

extern void OeStoreQuery_set_ifexists(T _this_,  bool ifexists);
extern bool OeStoreQuery_is_ifexists(T _this_);

extern void OeStoreQuery_set_take(T _this_,  bool is_take);
extern bool OeStoreQuery_is_take(T _this_);

extern void OeStoreQuery_set_tuple(T _this_,  bool is_tuple);
extern bool OeStoreQuery_is_tuple(T _this_);

extern size_t OeStoreQuery_get_max_results(T);

Arena_T OeStoreQuery_get_arena(T);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

