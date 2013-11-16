/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "OeStoreQuery.h"
#include "Oec_AList.h"

#define T OeStoreQuery

T OeStoreQuery_new(Arena_T arena, size_t nitems) {
    assert(arena);
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    _this_->max_items_to_return = nitems;
    _this_->results = Oec_AList_list(arena, NULL);
    return _this_;
}

//
//results
//
void OeStoreQuery_add_store_holder(T _this_,  OeStoreHolder holder) {

    _this_->results = Oec_AList_append( _this_->results,
                                        Oec_AList_list(_this_->arena, holder, NULL) );
}
OeStoreHolder OeStoreQuery_get_store_holder(T _this_, size_t idx) {
    return (OeStoreHolder) Oec_AList_get( _this_->results, idx );
}

void OeStoreQuery_reset_nitems_found(T _this_) {
    _this_->results = Oec_AList_list(_this_->arena, NULL);
}
size_t OeStoreQuery_get_nitems_found(T _this_) {
    return Oec_AList_length(_this_->results);
}

void OeStoreQuery_set_exists(T _this_,  bool exists) {
    _this_->exists = exists;
}
bool OeStoreQuery_exists(T _this_) {
    return _this_->exists;
}

//
//query
//

void OeStoreQuery_set_value_lists(T _this_,  DataObjectList value_lists) {
    assert(value_lists);
    _this_->value_lists = value_lists;
}
DataObjectList OeStoreQuery_get_value_lists(T _this_) {
    assert(_this_->value_lists);
    return _this_->value_lists;
}

void OeStoreQuery_set_owner(T _this_,  oe_id oid) {
    _this_->owner_id = oid;
}
oe_id OeStoreQuery_get_owner_id(T _this_) {
    return _this_->owner_id;
}

void OeStoreQuery_set_ifexists(T _this_,  bool ifexists) {
    _this_->ifexists = ifexists;
}
bool OeStoreQuery_is_ifexists(T _this_) {
    return _this_->ifexists;
}

void OeStoreQuery_set_take(T _this_,  bool is_take) {
    _this_->take = is_take;
}
bool OeStoreQuery_is_take(T _this_) {
    return _this_->take;
}

void OeStoreQuery_set_tuple(T _this_,  bool is_tuple) {
    _this_->is_tuple = is_tuple;
}
bool OeStoreQuery_is_tuple(T _this_) {
    return _this_->is_tuple;
}

Arena_T OeStoreQuery_get_arena(T _this_) {
    return _this_->arena;
}

size_t OeStoreQuery_get_max_results(T _this_) {
    return _this_->max_items_to_return;
}

#undef T

