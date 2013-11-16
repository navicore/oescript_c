/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "OeStore.h"
#include "OeStoreHolder.h"
#include "oe_common.h"

#define T OeStoreHolder

static DataObject _getdata(T _this_) {
    if (!_this_->data) {
        _this_->data = DataObject_new(_this_->arena);
    }
    return _this_->data;
}

int _unmarshal(T _this_, char *buffer);

T OeStoreHolder_new_with_data( Arena_T arena, DataObject data ) {

    assert(arena);

    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    _this_->data = data;
    return _this_;
}

T OeStoreHolder_new_from_buffer( Arena_T arena, char *buf ) {
    T h = OeStoreHolder_new(arena);
    _unmarshal(h, buf);
    return h;
}

T OeStoreHolder_new(Arena_T arena) {
    assert(arena);
    T h = OeStoreHolder_new_with_data( arena, NULL );
    return h;
}

int OeStoreHolder_marshal(T _this_, char **bufferpp, int *buffer_len, Arena_T arena) {

    int bufflen, mlen;
    char *databuff;

    int buffsize;

    buffsize = sizeof(struct store_holder_T);
    size_t number_of_props = DataObject_get_nattrs(_this_->data);
    for ( Iterator iter = DataObject_iterator(_this_->data, true);
          Iterator_hasMore(iter);) {
        buffsize = buffsize + strlen((char *) Iterator_next(iter)) + 1;
    }
    buffsize = buffsize + DataObject_get_nbytes(_this_->data) + 1;

    if (arena) {
        databuff = Arena_calloc(arena, 1, buffsize, __FILE__, __LINE__);
    } else {
        databuff = Mem_calloc(1, buffsize, __FILE__, __LINE__);
    }

    //copy every struct member to the buffer
    mlen = sizeof(int);
    memcpy(databuff, &_this_->lock, mlen);
    bufflen = mlen;

    mlen = sizeof(bool);
    memcpy(databuff + bufflen, &_this_->is_tuple, mlen);
    bufflen += mlen;

    mlen = sizeof(oe_id);
    memcpy(databuff + bufflen, &_this_->lease_id, mlen);
    bufflen += mlen;

    mlen = sizeof(oe_id);
    memcpy(databuff + bufflen, &_this_->txn_id, mlen);
    bufflen += mlen;

    mlen = sizeof(oe_time);
    memcpy(databuff + bufflen, &_this_->lease_expire_time, mlen);
    bufflen += mlen;

    mlen = sizeof(int);
    size_t nbytes =  DataObject_get_nbytes(_this_->data);
    memcpy(databuff + bufflen, &nbytes, mlen);
    bufflen += mlen;

    //mlen = sizeof(int);
    memcpy(databuff + bufflen, &number_of_props, mlen);
    bufflen += mlen;

    for ( Iterator iter = DataObject_iterator(_this_->data, true);
          Iterator_hasMore(iter);) {
        char *p = Iterator_next(iter);
        mlen = strlen((char *) p) + 1;
        memcpy(databuff + bufflen, p, mlen);
        bufflen += mlen;
    }

    if (nbytes > 0) {
        memcpy(databuff + bufflen, DataObject_get_bytes( _this_->data), nbytes);
        bufflen += nbytes;
    }

    *bufferpp = databuff;
    *buffer_len = bufflen;

    return 0;
}
OESTORE_SKIP_STATUS OeStoreHolder_skip(T _this_) {

    oe_time exptime = OeStoreHolder_get_lease_expire_time(_this_);

    if ( exptime != OEC_FOREVER && exptime <= oec_get_time_milliseconds() ) {
        return OESTORE_SKIP;
    }
    if ( OeStoreHolder_get_lock( _this_ ) == OESTORE_TAKE_LOCK ) {
        return OESTORE_SKIP;
    }
    if ( OeStoreHolder_get_lock( _this_ ) == OESTORE_WRITE_LOCK ) {
        return OESTORE_SKIP_BUT_EXISTS;
    }
    return OESTORE_DO_NOT_SKIP;
}

int _unmarshal(T _this_, char *buffer) {

    //todo: pos macro
    _this_->lock                = *((int *) buffer);
    int pos = sizeof(int);

    _this_->is_tuple            = *((bool *) (buffer + pos));
    pos += sizeof(bool);

    _this_->lease_id            = *((oe_id *) (buffer + pos));
    pos += sizeof(oe_id);

    _this_->txn_id              = *((oe_id *) (buffer + pos));
    pos += sizeof(oe_id);

    _this_->lease_expire_time   = *((oe_time *) (buffer + pos));
    pos += sizeof(oe_time);


    int nbytes                  = *((int *) (buffer + pos));
    pos += sizeof(int);

    size_t number_of_props      = *((int *) (buffer + pos));
    pos += sizeof(int);

    int i;
    DataObject props = _getdata(_this_);
    for (i = 0; i < number_of_props; i++) {
        oe_scalar prop = (oe_scalar) buffer + pos;
        DataObject_add_attr(props, prop);
        pos += strlen((char *) prop) + 1;
    }
    DataObject_set_nbytes(props, nbytes);

    if (nbytes > 0) {
        DataObject_set_bytes(props, buffer + pos);
    }

    return 0;
}

#define LID_POS sizeof(int) + sizeof(bool)
#define TID_POS LID_POS + sizeof(oe_id)
#define EXP_POS TID_POS + sizeof(oe_id)

oe_id OeStoreHolder_peek_lease_id(char *buffer) {
    //todo: macro
    int pos = LID_POS;
    oe_id lid = *((oe_id *) (buffer + pos));
    return lid;
}
oe_id OeStoreHolder_peek_txn_id(char *buffer) {
    //todo: macro
    int pos = TID_POS;
    oe_id oid = *((oe_id *) (buffer + pos));
    return oid;
}

oe_time OeStoreHolder_peek_lease_exp_time (char *buffer) {
    //todo: macro
    int pos = EXP_POS;
    oe_time time = *((oe_time *) (buffer + pos));
    return time;
}

void OeStoreHolder_set_tuple(T _this_,  bool is_tuple) {
    _this_->is_tuple = is_tuple;
}
bool OeStoreHolder_is_tuple(T _this_) {
    return _this_->is_tuple;
}

void OeStoreHolder_set_lock(T _this_,  OESTORE_LOCK lock) {
    _this_->lock = lock;
}
OESTORE_LOCK OeStoreHolder_get_lock(T _this_) {
    return _this_->lock;
}

void OeStoreHolder_set_txn_id(T _this_,  oe_id txn_id) {
    _this_->txn_id = txn_id;
}
oe_id OeStoreHolder_get_txn_id(T _this_) {
    return _this_->txn_id;
}

void OeStoreHolder_set_lease_expire_time(T _this_,  oe_time time) {
    _this_->lease_expire_time = time;
}
oe_time OeStoreHolder_get_lease_expire_time(T _this_) {
    return _this_->lease_expire_time;
}

DataObject OeStoreHolder_get_data(T _this_) {
    return _getdata(_this_);
}
void OeStoreHolder_set_data(T _this_, DataObject props) {
    _this_->data = props;
}

void OeStoreHolder_set_lease_id(T _this_,  oe_id lid) {
    _this_->lease_id = lid;
}
oe_id OeStoreHolder_get_lease_id(T _this_) {
    return _this_->lease_id;
}

#undef T

