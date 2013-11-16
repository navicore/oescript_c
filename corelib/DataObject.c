/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "DataObject.h"
#include "Oec_AList.h"
#include "arena.h"
#include "assert.h"
#include "oec_values.h"
#include "Iterator.h"
#include "ArrayIterator.h"
#include "oec_log.h"

#define T DataObject

// a list of attributes an an optional attachment
//
// this is the fundamental internal representation of user data.  its
// name is a harmless private joke :)

T DataObject_new(Arena_T arena) {
    assert(arena);
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    _this_->strings = Oec_AList_list(arena, NULL);
    _this_->always_cpy = false;
    return _this_;
}

/**
 * use this to turn on and off the forcing of the sdo to 
 * use its own arena to make copies of attrs and bytes you 
 * are adding.  normally you won't set this since most of 
 * your objects will be under the same arena as the sdo. 
 * 
 * @author esweeney (11/11/2010)
 * 
 * @param _this_ 
 * @param hint 
 */
void DataObject_set_always_copy(T _this_, bool hint) {
    _this_->always_cpy = hint;
}

static char *_sdo_bytes_cpy(T _this_, const char *bytes, size_t len)
{
    if (!_this_->always_cpy) return (char *) bytes;

    char *cpy;

    if (!(cpy = (char *) Arena_alloc(_this_->arena, len, __FILE__, __LINE__))) return 0;
    memcpy(cpy,bytes,len);
    return cpy;
}

static oe_scalar _sdo_cpy(T _this_, const oe_scalar str)
{
    if (!_this_->always_cpy) return str;

    size_t len;
    oe_scalar cpy;

    len = strlen(str) + 1;
    if (!(cpy = (oe_scalar) Arena_alloc(_this_->arena, len, __FILE__, __LINE__))) return 0;
    memcpy(cpy,str,len);
    return cpy;
}

size_t DataObject_get_nattrs(T _this_) {
    return(size_t) Oec_AList_length( _this_->strings );
}
void DataObject_add_attr(T _this_, oe_scalar value) {
    _this_->array_cache = NULL;
    oe_scalar lvalue = _sdo_cpy(_this_, value);
    _this_->strings = Oec_AList_append( _this_->strings,
                                        Oec_AList_list(_this_->arena, lvalue, NULL) );
}
void DataObject_set_nbytes(T _this_, size_t len) {
    assert( !_this_->nbytes );
    _this_->nbytes = len;
}
size_t DataObject_get_nbytes(T _this_) {
    return _this_->nbytes;
}
void DataObject_set_bytes(T _this_, char *bytes) {
    assert( !_this_->bytes );
    assert( _this_->nbytes ); //must set nbytes first
    char *lbytes = _sdo_bytes_cpy(_this_, bytes, _this_->nbytes);
    _this_->bytes = lbytes;
}
char *DataObject_get_bytes(T _this_) {
    assert( _this_->bytes );
    return _this_->bytes;
}
static ArrayIterator _dotoArrayIterator(T _this_) {
    if (!_this_->array_cache) {
        _this_->array_cache = Oec_AList_toArray( _this_->strings, _this_->arena, NULL );
    }
    return ArrayIterator_new( _this_->array_cache );
}
static ArrayIterator _dotoArrayIterator_no_arena(T _this_) {
    return ArrayIterator_new( Oec_AList_toArray( _this_->strings, NULL, NULL ) );
}

oe_scalar *DataObject_toArray(T _this_) {
    return (oe_scalar *) Oec_AList_toArray( _this_->strings, _this_->arena, NULL );
}

Iterator DataObject_iterator(T _this_, bool with_arena) {
    return Iterator_new((ITERATOR_next_call *) ArrayIterator_next, 
                        (ITERATOR_hasMore_call *) ArrayIterator_hasMore,
                        (ITERATOR_free_call *) ArrayIterator_free,
                        with_arena ? (ITERATOR_state_constructor *) _dotoArrayIterator: (ITERATOR_state_constructor *) _dotoArrayIterator_no_arena,
                        _this_, with_arena);
}

#undef T

