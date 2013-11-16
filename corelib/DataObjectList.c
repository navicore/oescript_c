/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "Iterator.h"
#include "ArrayIterator.h"
#include "DataObject.h"
#include "DataObjectList.h"
#include "Oec_AList.h"
#include "arena.h"

// a colletion for the fundamental internal representation of the
// user's data

#define T DataObjectList

T DataObjectList_new(Arena_T arena) {
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    _this_->lists = Oec_AList_list(arena, NULL);
    return _this_;
}

size_t DataObjectList_length(T _this_) {
    return (size_t) Oec_AList_length( _this_->lists );
}

DataObject DataObjectList_new_data_object(T _this_) {
    Arena_T arena = _this_->arena;
    DataObject slist = DataObject_new(arena);
    DataObjectList_add(_this_,slist);
    return slist;
}

void DataObjectList_add(T _this_, DataObject slist) {
    Arena_T arena = _this_->arena;
    _this_->lists = Oec_AList_append( _this_->lists,
                                      Oec_AList_list(arena, slist, NULL) );
}

//creates arena array, for iterator
static ArrayIterator _toArrayIterator(T _this_) {
    return ArrayIterator_new( Oec_AList_toArray( _this_->lists, _this_->arena, NULL ) );
}

//creates a malloc array, no arena, for iterator
static ArrayIterator _toArrayIterator_no_arena(T _this_) {
    return ArrayIterator_new( Oec_AList_toArray( _this_->lists, NULL, NULL ) );
}

/**
 * the iterator will work only as long as the src data object 
 * exists, but must be free'd independantly.  this way we can 
 * keep a data object around forever if we want but not leak 
 * iterators 
 */
Iterator DataObjectList_iterator(T _this_, bool with_arena) {
    return Iterator_new((ITERATOR_next_call *) ArrayIterator_next, 
                        (ITERATOR_hasMore_call *) ArrayIterator_hasMore,
                        (ITERATOR_free_call *) ArrayIterator_free,
                        with_arena ? (ITERATOR_state_constructor *) _toArrayIterator : (ITERATOR_state_constructor *) _toArrayIterator_no_arena,
                        _this_, with_arena);
}

#undef T

