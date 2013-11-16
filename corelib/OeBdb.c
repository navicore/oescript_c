/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "oe_common.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include DB_HEADER
#include "Oed_Dispatcher.h"
#include "OeBdb.h"
#include "OeBdb_resources.h"
#include "oec_threads.h"
#include "DataObject.h"
#include "OeStore.h"
#include "OeStoreHolder.h"
#include "Iterator.h"
#include "oec_log.h"

#define OES_KEY_SEP "~+"

//#define	OE_DB_TXN_TYPE			DB_TXN_SYNC
#define	OE_DB_TXN_TYPE			DB_TXN_SNAPSHOT

#define T OeBdb

#define ST_TYPE static
//#define ST_TYPE 

#define HT OeStoreHolder

#define OEBDB_DEFAULT_READ_BUFFER_SIZE 200
//#define OEBDB_DEFAULT_READ_BUFFER_SIZE 20000

struct T {

    //configured by oedb pkg
    DB *id_db           ;
    DB *txn_db          ;
    DB *holder_db       ;
    DB *object_db       ;
    DB *prop_idx        ;
    DB *owner_id_idx    ; //a txn is an owner.
    DB *expire_time_idx ;

    DB_SEQUENCE *seq;
    DB_ENV *env;

#ifdef OE_USE_THREADS
    pthread_mutex_t reaper_running_lock;
    pthread_cond_t  reaper_running_cond;
#endif
    bool reaper_running_flag;

    bool latch;
    pthread_mutex_t running_lock;
    pthread_cond_t  running_cond;
    bool running_flag;

    //configured by user
    const char *db_home_dir;
    int persist_level;
    void ( * fire_write_event ) ( DataObject, void * ); //todo: get out.  
    void *fire_write_event_arg; 
};

pthread_t watchdog_thread_id;
enum CMD {
    DEL, CLEAR, NONE
};

bool (_txn_is_active)(T _this_, oe_id tid, Arena_T arena);

ST_TYPE int _put_holder( T, DB_TXN *, OeStoreHolder, Arena_T );

ST_TYPE void init_running_flag(T _this_) {
    oe_pthread_mutex_init(&_this_->running_lock, NULL);
    oe_pthread_cond_init(&_this_->running_cond, NULL);
}

ST_TYPE bool _is_running(T _this_) {
    bool ret;
    oe_pthread_mutex_lock(&_this_->running_lock);
    ret = _this_->running_flag;
    oe_pthread_cond_signal(&_this_->running_cond);
    oe_pthread_mutex_unlock(&_this_->running_lock);
    return ret;
}

ST_TYPE void _set_running(T _this_, bool running) {
    oe_pthread_mutex_lock(&_this_->running_lock);
    _this_->running_flag = running;
    oe_pthread_cond_signal(&_this_->running_cond);
    oe_pthread_mutex_unlock(&_this_->running_lock);
}

ST_TYPE int _compare_oe_id(DB *dbp, const DBT *a, const DBT *b) {

    oe_id ai, bi;

    /* 
     * Returns: 
     * < 0 if a < b 
     * = 0 if a = b 
     * > 0 if a > b 
     */ 
    memcpy(&ai, a->data, sizeof(oe_id)); 
    memcpy(&bi, b->data, sizeof(oe_id)); 

    return(ai - bi); 
}

ST_TYPE int _compare_oe_time(DB *dbp, const DBT *a, const DBT *b) {

    oe_time ai, bi;

    /* 
     * Returns: 
     * < 0 if a < b 
     * = 0 if a = b 
     * > 0 if a > b 
     */ 
    memcpy(&ai, a->data, sizeof(oe_time)); 
    memcpy(&bi, b->data, sizeof(oe_time)); 

    return(ai - bi); 
}

ST_TYPE int _handlerc(int rc, DB_TXN *txn) {

    if (txn != NULL && rc != 0) {
        (void)txn->abort(txn);
    }
    switch (rc) {
    case DB_LOCK_DEADLOCK:
        return OE_STORE_PLEASE_RETRY;
    case DB_NOTFOUND:
        return OE_STORE_ITEM_NOTFOUND;
    }
    return rc;
}

ST_TYPE void _init_reaper_running_flag(T _this_) {
    oe_pthread_mutex_init(&_this_->reaper_running_lock, NULL);
    oe_pthread_cond_init(&_this_->reaper_running_cond, NULL);
    _this_->reaper_running_flag = false;
}

ST_TYPE bool _is_reaper_running(T _this_) {
    bool ret;
    oe_pthread_mutex_lock(&_this_->reaper_running_lock);
    ret = _this_->reaper_running_flag;
    oe_pthread_cond_signal(&_this_->reaper_running_cond);
    oe_pthread_mutex_unlock(&_this_->reaper_running_lock);
    return ret;
}
//ret true means you have successfully changed it.  if you successfully set running true you better set it false when done
ST_TYPE bool _set_reaper_running(T _this_, bool running) {
    assert(_this_);
    bool ret;
    oe_pthread_mutex_lock(&_this_->reaper_running_lock);
    if (_this_->reaper_running_flag) ret == false; //can't set it running if it is already.  someone else has started
    else {
        _this_->reaper_running_flag = running;
        ret = true;
    }
    oe_pthread_cond_signal(&_this_->reaper_running_cond);
    oe_pthread_mutex_unlock(&_this_->reaper_running_lock);
    return ret;
}

// -1 rc means error, quit
ST_TYPE int (_deadlock_detect)(void *_this_v, int type) {

    T _this_ = (T) _this_v;

    int cnt = 0;
    DB_ENV *env = _this_->env;

    int ret = env->lock_detect( env, 0, type, &cnt );

    if (ret) {
        OE_ERR( NULL, OEDB_STR_PRO_DL_ERR, db_strerror(ret) );
        return -1;
    }
    return cnt;
}

ST_TYPE void *_deadlock_watchdog(void *arg) {

    T _this_ = (T) arg;

    for ( ;; ) {

        if ( !_is_running(_this_) ) return NULL;

        if ( _deadlock_detect( _this_, DB_LOCK_RANDOM ) ) {
            OE_DLOG(NULL, OEDB_WATCHDOG_CLEARED_LOCK);
        }
        sleep( 2 );

        if ( _deadlock_detect( _this_, DB_LOCK_EXPIRE ) > 0 ) {
            OE_DLOG(NULL, OEDB_WATCHDOG_CLEARED_EXPIRED_LOCK);
        }
        sleep( 2 );
    }
    return NULL;
}

ST_TYPE int _close_cursors(DBC **carray) {
    int ret = 0;
    for (; *carray; carray++) {
        DBC *c = *carray;
        int lret = c->close(c);
        if (lret) {
            ret = lret;
        }
    }
    return ret;
}

ST_TYPE int _finish_txn(DB_TXN *txn, bool commit) {
    if (txn == NULL) {
        return 0;
    }
    if (commit && txn != NULL && (txn->flags & TXN_DEADLOCK)) {
        (void)txn->abort(txn);
        return DB_LOCK_DEADLOCK;
    }
    int ret;
    if (commit) {
        ret = txn->commit(txn, 0);
        return ret;
    } else {
        txn->abort(txn);
        return 0;
    }
}

ST_TYPE int _get_with_cursor(Arena_T arena, DBC *cur, DBT *key, DBT *data, int opts ) {

    //todo: add some instrumentation here so that most reads work the first try

    data->flags = DB_DBT_USERMEM;
    int ret;
    data->ulen = OEBDB_DEFAULT_READ_BUFFER_SIZE;
    bool done = false;
    while ( !done ) {
        data->data = Arena_calloc(arena, 1, data->ulen, __FILE__, __LINE__);
        ret = cur->get( cur, key, data, opts );
        if ( ret == DB_BUFFER_SMALL ) {
            data->ulen = data->size;
        } else {
            done = true;
        }
    }

    return ret;
}

ST_TYPE int _get(Arena_T arena, DB *dbp, DB_TXN *txn, DBT *key, DBT *data, int opts ) {

    data->flags = DB_DBT_USERMEM;
    int ret;
    data->ulen = OEBDB_DEFAULT_READ_BUFFER_SIZE;
    bool done = false;
    while ( !done ) {
        data->data = Arena_calloc(arena, 1, data->ulen, __FILE__, __LINE__);
        ret = dbp->get(dbp, txn, key, data, opts);
        if ( ret == DB_BUFFER_SMALL ) {
            data->ulen = data->size;
        } else {
            done = true;
        }
    }
    return ret;
}

ST_TYPE int _update_locks_for_take(T _this_, OeStoreQuery query, DB_TXN *txn) {

    int ret;

    assert(txn);
    if (txn != NULL && (txn->flags & TXN_DEADLOCK)) {
        return OE_STORE_PLEASE_RETRY;
    }

    DBT key, data;
    memset(&key, 0, sizeof (DBT));
    memset(&data, 0, sizeof (DBT));
    key.flags = DB_DBT_APPMALLOC;

    Arena_T arena = OeStoreQuery_get_arena(query);
    for (int i = 0; i < OeStoreQuery_get_nitems_found(query) ; i++) {

        OeStoreHolder result = OeStoreQuery_get_store_holder(query, i);
        assert(result);
        oe_id lid;
        lid = OeStoreHolder_get_lease_id( result );
        key.data = &lid;
        key.size = sizeof(oe_id);

        DB *hdb = _this_->holder_db;

        ret = _get(arena, hdb, txn, &key, &data, DB_RMW);
        if (ret == DB_NOTFOUND) {
            return OE_STORE_PLEASE_RETRY; //todo: just remove from list if list > 1
        }
        if (ret) {
            return ret;
        }
        OeStoreHolder rholder = OeStoreHolder_new_from_buffer(arena, data.data);
        if (OeStoreHolder_skip(rholder) != OESTORE_DO_NOT_SKIP) {
            return OE_STORE_PLEASE_RETRY; //todo: just remove from list if list > 1
        }

        int ret = _put_holder(_this_, txn, result, arena);
        if (ret) {
            return ret;
        }
    }
    return 0;
}

ST_TYPE int _delete_items_for_take(T _this_, OeStoreQuery query, DB_TXN *txn) {

    int ret;

    assert(txn);
    if (txn != NULL && (txn->flags & TXN_DEADLOCK)) {
        return OE_STORE_PLEASE_RETRY;
    }

    DBT key, data;
    memset(&key, 0, sizeof (DBT));
    memset(&data, 0, sizeof (DBT));
    key.flags = DB_DBT_APPMALLOC;

    Arena_T arena = OeStoreQuery_get_arena(query);

    for (int i = 0; i < OeStoreQuery_get_nitems_found(query) ; i++) {
        OeStoreHolder result = OeStoreQuery_get_store_holder(query, i);
        oe_id lid;
        lid = OeStoreHolder_get_lease_id( result );
        key.data = &lid;
        key.size = sizeof(oe_id);

        DB *hdb = _this_->holder_db;

        ret = _get(arena, hdb, txn, &key, &data, DB_RMW);
        if (ret == DB_NOTFOUND) {
            return OE_STORE_PLEASE_RETRY; //todo: just remove from list if list > 1
        }
        if (ret) {
            return ret;
        }
        OeStoreHolder rholder = OeStoreHolder_new_from_buffer(arena, data.data);
        if (OeStoreHolder_skip(rholder) != OESTORE_DO_NOT_SKIP) {
            return OE_STORE_PLEASE_RETRY; //todo: just remove from list if list > 1
        }

        ret = hdb->del(hdb, txn, &key, 0);
        if (ret) {
            return ret;
        }
    }
    return 0;
}

ST_TYPE int _position_tuple_cursors(Arena_T arena, DataObject values, DBC **cursor_array) {
    //cursor array size should already be calculated to omit '_' wildcards
    int ret;
    DBT key, data;
    memset(&key, 0, sizeof (DBT));
    memset(&data, 0, sizeof (DBT));
    data.flags = DB_DBT_MALLOC;

    int idx = 0;
    int pos = 0;
    int sz = DataObject_get_nattrs(values);
    for (Iterator iter = DataObject_iterator(values, true); Iterator_hasMore(iter); pos++) {
        //oe_scalar *skey = Iterator_next(iter);
        oe_scalar skey = Iterator_next(iter);
#ifdef OE_TRACE
        OE_DLOG(NULL, OEDB_POS_TUPLE_CURSORS, skey);
#endif /* OE_TRACE'ing */
        if (strcmp(skey, __OESTORE_WILDCARD__) == 0) continue; //wildcard
        char *keystr = oec_str_concat(arena, 5,
                                      skey,
                                      OES_KEY_SEP,
                                      oec_itoa(arena, pos),
                                      OES_KEY_SEP,
                                      oec_itoa(arena, sz));
        key.data = keystr;
        key.size = strlen(keystr) + 1;
        ret = _get_with_cursor(arena, cursor_array[idx++], &key, &data, DB_SET);
        if (ret)  return ret;
    }
    return 0;
}

ST_TYPE int _position_map_cursors(Arena_T arena, DataObject values, DBC **cursor_array) {

    int ret;
    DBT key, data;
    memset(&key, 0, sizeof (DBT));
    memset(&data, 0, sizeof (DBT));
    data.flags = DB_DBT_MALLOC;

    int idx = 0;
    int sz = DataObject_get_nattrs(values);
    if (sz <= 0) return -1;
    if (sz % 2) return -1; //must be in pairs
    for (Iterator iter = DataObject_iterator(values, true); Iterator_hasMore(iter);) {
        //oe_scalar *dkey = Iterator_next(iter);
        oe_scalar dkey = Iterator_next(iter);
        //oe_scalar *dvalue = Iterator_next(iter);
        oe_scalar dvalue = Iterator_next(iter);
        char *keystr = oec_str_concat(arena, 3, dkey, OES_KEY_SEP, dvalue);
        OE_TLOG(NULL, "OeBdb._position_map_cursors key: %s val: %s keystr: %s\n", dkey, dvalue, keystr);
        key.data = keystr;
        key.size = strlen(keystr) + 1;
        ret = _get_with_cursor(arena, cursor_array[idx++], &key, &data, DB_SET);
        if (ret)  return ret;
    }
    return 0;
}

static int _calc_nkeys(OeStoreQuery query, DataObject values) {
    int nvals = (int) DataObject_get_nattrs(values);
    int nkeys = nvals;
    if (OeStoreQuery_is_tuple(query)) {
        for (Iterator iter = DataObject_iterator(values, true);
            Iterator_hasMore(iter);) {
            char *skey = Iterator_next(iter);
            if (strcmp(skey, __OESTORE_WILDCARD__) == 0) nkeys--; //wildcard
        }
    } else {
        nkeys = nkeys / 2;
    }
    return nkeys;
}

ST_TYPE int (_get_one)(T _this_, OeStoreQuery query,
                       DataObject data, DB_TXN *txn_holder) {

    OE_TLOG(NULL, "OeBdb._get_one\n");
    assert(_this_);
    assert(txn_holder);
    assert(query);
    Arena_T arena = OeStoreQuery_get_arena(query);
    assert(arena);
    assert(data);

    int ret;

    int nkeys = _calc_nkeys(query, data);

    if (nkeys <= 0) return -1;
    DBC **cursor_array = Arena_calloc(arena, nkeys + 1,
                                      sizeof(DBC), __FILE__, __LINE__);

    //open prop cursors
    DB *idx = _this_->prop_idx;
    for (int i = 0; i < nkeys; i++) {
        int ret = idx->cursor(idx, txn_holder,
                              (DBC **) &cursor_array[i], OE_DB_TXN_TYPE );
        if (ret) {
            idx->err(idx, ret, OEDB_STR_OPEN_CUR_FAILED);
            _close_cursors(cursor_array);
            return ret;
        }
    }
    cursor_array[nkeys] = NULL;

    //position prop cursors
    if (OeStoreQuery_is_tuple(query)) {
        ret = _position_tuple_cursors(arena, data, cursor_array);
    } else {
        ret = _position_map_cursors(arena, data, cursor_array);
    }
    if (ret) {
        _close_cursors(cursor_array);
        return ret;
    }

    DBC *join_cur;
    DB *db = _this_->holder_db;
    ret = db->join(db, cursor_array, &join_cur, 0);
    if (ret) {
        join_cur->close(join_cur);
        _close_cursors(cursor_array);
        return ret;
    }

    // real skip locked behavior is needed here.  this is a *bag*
    DBT key, sdata;
    memset(&key, 0, sizeof (DBT));
    memset(&sdata, 0, sizeof (DBT));
    sdata.flags = DB_DBT_MALLOC;

    sdata.flags = DB_DBT_USERMEM;
    for (;;) {

        ret = _get_with_cursor(arena, join_cur,&key, &sdata, 0);

        if (ret) {
            join_cur->close(join_cur);
            _close_cursors(cursor_array);
            return ret;
        }

        OeStoreHolder rholder = OeStoreHolder_new_from_buffer(arena, sdata.data);

        OESTORE_SKIP_STATUS skip = OeStoreHolder_skip(rholder);
        switch (skip) {
        case OESTORE_SKIP:
            continue;
        case OESTORE_SKIP_BUT_EXISTS:
            OeStoreQuery_set_exists(query, true);
            continue;
        case OESTORE_DO_NOT_SKIP:
        default:
            break;
        }

        //if take, update lock
        if (OeStoreQuery_is_take(query)) {
            OeStoreHolder_set_lock( rholder, OESTORE_TAKE_LOCK);
            OeStoreHolder_set_txn_id(rholder, OeStoreQuery_get_owner_id(query));
        }

        OeStoreQuery_add_store_holder(query, rholder);
        OeStoreQuery_set_exists(query, true);
        if (OeStoreQuery_get_nitems_found(query) >=
            OeStoreQuery_get_max_results(query)) {
            break;
        }
    }
    join_cur->close(join_cur);
    _close_cursors(cursor_array);

    return ret;
}

ST_TYPE int _try_put(T _this_, OeStoreHolder holder, Arena_T arena, DB_TXN *txn) {

    oe_id tid = OeStoreHolder_get_txn_id(holder);
    if (tid && !_txn_is_active(_this_, tid, arena)) {
        return OESTORE_INACTIVE_TXN;
    }

    int ret = _put_holder(_this_, txn, holder, arena);
    if (!ret) {

        if (!(txn->flags & TXN_DEADLOCK)) {
            ret = txn->commit(txn, 0);
            txn = NULL;
        } else {
            ret = DB_LOCK_DEADLOCK;
        }
        if (!ret) {
            if ( _this_->fire_write_event && OeStoreHolder_get_txn_id( holder ) == 0) {
                OE_TLOG( NULL, "OeBdb _try_put firing write event\n");
                _this_->fire_write_event( OeStoreHolder_get_data( holder ),
                                          _this_->fire_write_event_arg );
            }
        }
    }

    return ret;
}

ST_TYPE int _put_holder(T _this_, DB_TXN *txn, OeStoreHolder holder, Arena_T arena) {

#ifdef OE_TRACE
    OE_DLOG( NULL, OEDB_PUT_HOLDER);
#endif /* OE_TRACE'ing */
    assert(_this_);
    assert(arena);
    assert(holder);

    if (txn != NULL && (txn->flags & TXN_DEADLOCK)) {
        return DB_LOCK_DEADLOCK;
    }
    assert(txn);
    assert(holder);

    int ret;
    char *buffer;
    int buffer_len;

    ret = OeStoreHolder_marshal(holder, &buffer, &buffer_len, arena);
    if (ret) {
        OE_ERR(NULL, OEDB_STR_NOT_MARSHAL_HDR);
        return ret;
    }

    DBT key, data;
    memset(&key, 0, sizeof (DBT));
    memset(&data, 0, sizeof (DBT));

    oe_id *lid;
    lid = Arena_alloc(arena, sizeof *lid, __FILE__, __LINE__ );
    *lid = OeStoreHolder_get_lease_id( holder );
    key.data = lid;
    key.size = sizeof *lid;
    key.flags = DB_DBT_APPMALLOC;

    data.data = buffer;
    data.size = buffer_len;
    data.flags = DB_DBT_APPMALLOC;

    ret = _this_->holder_db->put(_this_->holder_db, txn, &key, &data, 0);

    return ret;
}

ST_TYPE int _finish_oetxn(T _this_, Arena_T, OESTORE_TXN*, DB_TXN*, OESTORE_LOCK,
                          OESTORE_LOCK, OESTORE_TXN_STATUS, Oec_AList *, int);

ST_TYPE int _put_oetxn            (OeBdb, OESTORE_TXN *, DB_TXN *, Arena_T);

ST_TYPE int _marshal_txn(OESTORE_TXN *oetxn, char **bufferpp, int *buffer_len, Arena_T arena) {

    assert(arena);

    int buffsize, bufflen, mlen;
    char *databuff;

    //get the buffer

    buffsize = sizeof(int) + sizeof(oe_time) + sizeof(oe_time) + sizeof(oe_time);

    //databuff = Mem_alloc(buffsize, __FILE__, __LINE__);
    databuff = Arena_alloc( arena, buffsize, __FILE__, __LINE__ );
    memset(databuff, 0, buffsize);

    //copy every struct member to the buffer

    mlen = sizeof(int);
    memcpy(databuff, &oetxn->status, mlen);
    bufflen = mlen;

    mlen = sizeof(oe_time);
    memcpy(databuff + bufflen, &oetxn->expire_time, mlen);
    bufflen += mlen;

    mlen = sizeof(oe_time);
    memcpy(databuff + bufflen, &oetxn->begin_time, mlen);
    bufflen += mlen;

    mlen = sizeof(oe_time);
    memcpy(databuff + bufflen, &oetxn->end_time, mlen);
    bufflen += mlen;

    *bufferpp = databuff;
    *buffer_len = bufflen;

    //note, you do not store the tid since that is stored in the db key

    return 0;
}

ST_TYPE int _unmarshal_txn(OESTORE_TXN* oetxn, char* buffer) {

    oetxn->status           = *((int *) buffer);
    int pos = sizeof(int);

    oetxn->expire_time      = *((oe_time *) (buffer + pos));
    pos += sizeof(oe_time);

    oetxn->begin_time       = *((oe_time *) (buffer + pos));
    pos += sizeof(oe_time);

    oetxn->end_time         = *((oe_time *) (buffer + pos));
    pos += sizeof(oe_time);

    return 0;
}

ST_TYPE int _put_oetxn(T _this_, OESTORE_TXN *oetxn, DB_TXN *txn, Arena_T arena) {

    if (txn != NULL && (txn->flags & TXN_DEADLOCK)) {
        return DB_LOCK_DEADLOCK;
    }
    //marshall txn context
    int     ret;
    char    *buffer;
    int     buffer_len;
    ret = _marshal_txn(oetxn, &buffer, &buffer_len, arena);

    //record txn
    DBT key, data;
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    key.data = &oetxn->tid;
    key.size = sizeof(oe_id);

    data.data = buffer;
    data.size = buffer_len;

    ret = _this_->txn_db->put(_this_->txn_db, txn, &key, &data, 0);

    //Mem_free(buffer, __FILE__, __LINE__);

    return ret;
}

ST_TYPE int _get_txn(T _this_, Arena_T arena, OESTORE_TXN *oetxn, DB_TXN *txn) {

    if (txn != NULL && (txn->flags & TXN_DEADLOCK)) {
        return DB_LOCK_DEADLOCK;
    }
    //record txn
    DBT key, data;
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    oe_id tid = oetxn->tid;
    key.data = &tid;
    key.size = sizeof(oe_id);

    data.flags = DB_DBT_MALLOC;

    DB *txndb = _this_->txn_db;
    //int ret = txndb->get(txndb, txn, &key, &data, DB_RMW);
    int ret;
    if (txn) {
        ret = _get(arena, txndb, txn, &key, &data, DB_RMW);
    } else {
        //ret = _get(arena, txndb, txn, &key, &data, NULL);
        ret = _get(arena, txndb, txn, &key, &data, 0);
    }

    if (!ret) {
        _unmarshal_txn(oetxn, data.data);
        //FREE(data.data);
    }

    return ret;
}

ST_TYPE int _get_first_a(Arena_T arena, DBC *cur, DBT *key, DBT *data ) {
    return _get_with_cursor( arena, cur, key, data, DB_RMW | DB_SET );
}
ST_TYPE int _get_next_a(Arena_T arena, DBC *cur, DBT *key, DBT *data ) {
    return _get_with_cursor( arena, cur, key, data, DB_RMW | DB_NEXT_DUP );
}

ST_TYPE int _finish_oetxn(T _this_,
                          Arena_T            arena,
                          OESTORE_TXN       *oetxn_p,
                          DB_TXN            *txn,
                          OESTORE_LOCK       delete_lock_type,
                          OESTORE_LOCK       clear_lock_type,
                          OESTORE_TXN_STATUS txn_status,
                          Oec_AList          *unlocked_holders,
                          int                expected_enlisted) {

    if (txn != NULL && (txn->flags & TXN_DEADLOCK)) {
        return DB_LOCK_DEADLOCK;
    }

    if ( oetxn_p->status != OESTORE_TXN_ACTIVE ) {
        return OESTORE_ALREADY_COMPLETE_ERR;
    }

    int ret;

    oe_id tid = oetxn_p->tid;
    if (tid == 0) {
        return OESTORE_IS_NULL_ERR;
    }

    DBC *cur;

    DBT key, data;
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    key.data = &tid;
    key.size = sizeof(oe_id);

    DB *ididx = _this_->owner_id_idx;
    if ((ret = ididx->cursor(ididx, txn, &cur, OE_DB_TXN_TYPE )) != 0) {
        return ret;
    }

    int updated_items = 0;
    for ( ret = _get_first_a(arena, cur, &key, &data); ret == 0; ret = _get_next_a(arena, cur, &key, &data ) ) {

        updated_items++;

        OeStoreHolder rholder = OeStoreHolder_new_from_buffer(arena, data.data);

        OESTORE_LOCK lock = OeStoreHolder_get_lock(rholder);

        enum CMD cmd;

        if (lock == delete_lock_type) {
            cmd = DEL;
        } else if (lock == clear_lock_type) {
            cmd = CLEAR;
        } else {
            cmd = NONE;
        }

        int lret;

        switch (cmd) {
        
        case DEL:

            lret = cur->del(cur, 0);
            if (lret) {
                cur->close(cur);
                return lret;
            }
            break;

        case CLEAR:

            OeStoreHolder_set_txn_id(rholder, 0);
            OeStoreHolder_set_lock(rholder, OESTORE_UNLOCKED);

            *unlocked_holders = Oec_AList_append(*unlocked_holders, Oec_AList_list(arena, OeStoreHolder_get_data(rholder), NULL) );

            char *buffer;
            int buffer_len;

            ret = OeStoreHolder_marshal(rholder, &buffer, &buffer_len, arena);

            oe_id lid = OeStoreHolder_get_lease_id(rholder);
            DBT lkey;
            memset(&lkey, 0, sizeof(DBT));
            lkey.data = &lid;
            lkey.size = sizeof lid;

            DBT ldata;
            memset(&ldata, 0, sizeof(DBT));
            ldata.data = buffer;
            ldata.size = buffer_len;
            ldata.flags = DB_DBT_USERMEM;

            ret = _this_->holder_db->put(_this_->holder_db, txn, &lkey, &ldata, 0);
            if (ret) {
                cur->close(cur);
                return ret;
            }

            break;

        default:
            //panic
            _this_->txn_db->errx(_this_->txn_db, OEDB_STR_FOUND_UNLOCKED_COMMIT);
            ret = -1;
            cur->close(cur);
            return ret;
        }
    }

    int lret = cur->close(cur);
    if (lret == DB_LOCK_DEADLOCK) {
        return lret;
    }

    //compair updated_items to expected (-1 means don't test)
    if (expected_enlisted != -1 && updated_items != expected_enlisted) {
        OE_DLOG(NULL, OEDB_WRONG_ENLISTED_COUNT, expected_enlisted, updated_items);
        return OESTORE_ENLISTED_COUNT_INCORRECT;
    }

    oe_time now = oec_get_time_milliseconds();

    oetxn_p->end_time       = now;
    oetxn_p->status         = txn_status;
    oetxn_p->expire_time    = now;

    if (ret == DB_NOTFOUND) {
        return 0; //empty txn is ok
    }

    return ret;
}

ST_TYPE int _close_db(DB *dbs) {
    if (dbs != NULL) {
        int ret = dbs->close(dbs, 0);
        if (ret != 0) {
            dbs->err(dbs, ret, "oedb");
            return ret;
        }
    }
    return 0;
}

ST_TYPE int _open_database(T _this_, DB_TXN *txn, DB **dbpp, const char *file_name,
                           bool allow_dupes, bool rewritable, void *compair_fun, bool sortdups) {
    assert(_this_);
    assert(txn);
    assert(dbpp);

    DB *dbp;
    u_int32_t open_flags;
    int ret;

    ret = db_create(dbpp, _this_->env, 0);
    dbp = *dbpp;

    if (ret != 0) {
        OE_ERR(NULL,"%s\n", db_strerror(ret));
        return ret;
    }

    if (compair_fun) {
        dbp->set_bt_compare(dbp, compair_fun);
    }
    if (compair_fun && sortdups) {
        dbp->set_dup_compare(dbp, compair_fun);
    }

    dbp->set_errpfx(dbp, file_name);

    u_int32_t db_flags = 0;
    if ( allow_dupes ) db_flags = DB_DUPSORT;
    if ( db_flags ) {
        dbp->set_flags( dbp, db_flags );
    }

    open_flags =
    DB_CREATE       |
//#ifdef OE_USE_THREADS
    DB_THREAD       |
//#endif
    DB_MULTIVERSION;

    if (_this_->persist_level > 1) {
        ret = dbp->open(dbp, txn, file_name, NULL, DB_BTREE, open_flags, 0664);
    } else {
        DB_MPOOLFILE *mpf = dbp->get_mpf( dbp );
        ret = mpf->set_flags( mpf, DB_MPOOL_NOFILE, 1 );
        assert(!ret);
        ret = dbp->open( dbp, txn, NULL, NULL, DB_BTREE, open_flags, 0664 );
    }
    if (ret != 0) {
        dbp->err(dbp, ret, OEDB_STR_DB_OPEN_FAILED, file_name);
        return(ret);
    }
    return 0;
}

ST_TYPE void _log_msg(const DB_ENV *env, const char *text) {
    OE_ILOG(NULL, OEDB_STR_LOG, text );
}
ST_TYPE void _log_err(const DB_ENV *env, const char *prefix, const char *text) {
    OE_ERR(NULL OEDB_STR_ERRLOG, prefix, text );
}

ST_TYPE int _set_map_props(OeStoreHolder holder,
                           DB *dbp, const DBT *pkey, const DBT *pdata, DBT *skey) {
    Arena_T arena = Oed_get_arena(); 
    DataObject data = OeStoreHolder_get_data(holder);
    size_t sz = DataObject_get_nattrs( data );
    if (sz <= 0) return OE_STORE_MISSING_ATTRS;
    if (sz % 2) return OE_STORE_ATTRS_NOT_PAIRED;

    DBT *tmpdbt;
    tmpdbt = Arena_calloc(arena, sz + 1, sizeof(DBT), __FILE__, __LINE__);
    size_t number_of_keys = 0;
    for ( Iterator iter =  DataObject_iterator(OeStoreHolder_get_data(holder), true);
        Iterator_hasMore(iter); ) {

        //oe_scalar *key =  Iterator_next(iter);
        oe_scalar key =  Iterator_next(iter);
        //oe_scalar *val =  Iterator_next(iter);
        oe_scalar val =  Iterator_next(iter);
        if (strcmp(key, __OESTORE_NULL__) == 0)
            continue; //an unmatchable (list or array prop)
        if (strcmp(val, __OESTORE_NULL__) == 0)
            continue; //an unmatchable (list or array prop)

        oe_scalar p = oec_str_concat(arena, 3, key, OES_KEY_SEP, val);
#ifdef OE_TRACE
        OE_DLOG(NULL, OEDB_SET_MAP_PROPS, p);
#endif /* OE_TRACE'ing */
        u_int32_t len = (u_int32_t) strlen((char *) p) + 1;
        tmpdbt[number_of_keys].size = len;
        tmpdbt[number_of_keys].data = p;
        number_of_keys++;
    }

    memset(skey, 0, sizeof *skey);
    skey->flags = DB_DBT_MULTIPLE;
    skey->data = tmpdbt;
    skey->size = number_of_keys;

    return(0);
}

ST_TYPE int _set_tuple_props(OeStoreHolder holder,
                             DB *dbp, const DBT *pkey, const DBT *pdata, DBT *skey) {

    Arena_T arena = Oed_get_arena(); 
    DataObject data = OeStoreHolder_get_data(holder);
    size_t number_of_attrs = DataObject_get_nattrs( data );
    size_t number_of_keys = number_of_attrs;

    DBT *tmpdbt;
    tmpdbt = Arena_calloc(arena, number_of_keys + 1, sizeof(DBT), __FILE__, __LINE__);
    int idx = 0;
    int pos = 0;
    for ( Iterator iter = DataObject_iterator(data, true);
        Iterator_hasMore(iter); pos++) {

        //oe_scalar *key = Iterator_next(iter);
        oe_scalar key = Iterator_next(iter);
        if (strcmp(key, "_") == 0) {
            number_of_keys--;
            continue; //an unmatchable (list or array)
        }
        oe_scalar p = oec_str_concat(arena, 5,
                                     key,
                                     OES_KEY_SEP,
                                     oec_itoa(arena, pos),
                                     OES_KEY_SEP,
                                     oec_itoa(arena, number_of_attrs));
#ifdef OE_TRACE
        OE_DLOG(NULL, OEDB_SET_TUPLE_PROPS, p);
#endif /* OE_TRACE'ing */
        u_int32_t len = (u_int32_t) strlen((char *) p) + 1;
        tmpdbt[idx].size = len;
        tmpdbt[idx].data = p;
        idx++;
    }
    if (number_of_keys <= 0) return -1;

    memset(skey, 0, sizeof *skey);
    skey->flags = DB_DBT_MULTIPLE;
    skey->data = tmpdbt;
    skey->size = number_of_keys;

    return(0);
}

ST_TYPE int _set_props(DB *dbp, const DBT *pkey, const DBT *pdata, DBT *skey) {

#ifdef OE_TRACE
    OE_DLOG( NULL, OEDB_SET_PROPS);
#endif /* OE_TRACE'ing */

    oe_id oid = OeStoreHolder_peek_txn_id( pdata->data );
    if (oid != 0) {
        return DB_DONOTINDEX;
        /////////////////////////////////////////
        /////////////////////////////////////////
        /// if you don't index an item owned  ///
        /// by a txn then the 'ifexists' 'get'///
        /// calls can't wait for the txn to   ///
        /// finish.  'ifexists' is broken.    ///
        /////////////////////////////////////////
        /////////////////////////////////////////

        /////////////////////////////////////////
        /////////////////////////////////////////
        /// note, to fix 'ifexists', you need ///
        /// to mangle the props names and     ///
        /// then you'd have to look for both  ///
        /// mangled and unmangled in join...  ///
        /// not good.                         ///
        ///                                   ///
        /// the real solution, not great      ///
        /// but workable, is an 'ifexists'    ///
        /// matcher.                          ///
        /////////////////////////////////////////
        /////////////////////////////////////////

        /////////////////////////////////////////
        /////////////////////////////////////////
        /// or, index all items and 'skip' txn///
        /// owned items.  this is bad because ///
        /// i want txns to last forever and   ///
        /// the odd txn running a month or    ///
        /// more will keep being skipped.     ///
        /////////////////////////////////////////
        /////////////////////////////////////////
    }
    Arena_T arena = Oed_get_arena(); 
    OeStoreHolder holder = OeStoreHolder_new_from_buffer(arena, pdata->data);
    if (OeStoreHolder_is_tuple(holder)) {
        return _set_tuple_props(holder,dbp,pkey,pdata,skey);
    } else {
        return _set_map_props(holder,dbp,pkey,pdata,skey);
    }
}

ST_TYPE int _set_expire_time(DB *dbp, const DBT *pkey, const DBT *pdata, DBT *skey) {

    oe_id oid = OeStoreHolder_peek_txn_id( pdata->data );
    if (oid != 0) {
        return DB_DONOTINDEX;
    }
    oe_time exp_time = OeStoreHolder_peek_lease_exp_time( pdata->data );
    if (exp_time == OEC_FOREVER) {
        return DB_DONOTINDEX;
    }
    oe_time *timep;
    timep = Mem_alloc( sizeof *timep, __FILE__, __LINE__ );
    *timep = exp_time;

    memset(skey, 0, sizeof *skey);
    skey->flags = DB_DBT_APPMALLOC;
    skey->data = timep;
    skey->size = sizeof *timep;

    return(0);
}

ST_TYPE int _set_owner(DB *dbp, const DBT *pkey, const DBT *pdata, DBT *skey) {

    oe_id oid = OeStoreHolder_peek_txn_id( pdata->data );
    if (oid == 0) {
        return DB_DONOTINDEX;
    }
    oe_id *owner_id;
    owner_id = Mem_alloc( sizeof *owner_id, __FILE__, __LINE__ );
    *owner_id = oid;

    memset(skey, 0, sizeof *skey);
    skey->flags = DB_DBT_APPMALLOC;
    skey->data = owner_id;
    skey->size = sizeof *owner_id;

    return(0);
}

ST_TYPE int _get_first( DBC *cur, DBT *key, DBT *data ) {
    return cur->get( cur, key, data, DB_RMW | DB_SET_RANGE );
}
ST_TYPE int _get_prev( DBC *cur, DBT *key, DBT *data ) {
    return cur->get(cur, key, data, DB_RMW | DB_PREV );
}

///////////////////////////////////////////////////////////////////////////////
///                                                                         ///
///                                                                         ///
///                              BEGIN IFC                                  ///
///                                                                         ///
///                                                                         ///
///////////////////////////////////////////////////////////////////////////////

ST_TYPE oe_id (_create_id)(void *_this_v) {
    T _this_ = (T) _this_v;

    u_int32_t get_flags;
    if (_this_->persist_level > 2) {
        get_flags = DB_AUTO_COMMIT;
    } else {
        get_flags = DB_AUTO_COMMIT | DB_TXN_NOSYNC;
    }

    oe_id seqnum;
    for ( int i = 0 ; ; i++ ) { //no plan b if we can't gen id(s)
        int ret = _this_->seq->get(_this_->seq, NULL, 1, (int64_t*) &seqnum, get_flags);
        if (ret) {
            if ( ret == DB_LOCK_DEADLOCK ) {
                OE_DLOG( NULL, OEDB_STR_CREATE_ID_DL_RETRY, i + 1 );
            } else {
                OE_DLOG(NULL,OEDB_STR_CR_ID_DL_RETRY_ERR, i + 1, db_strerror(ret));
            }
            continue;
        }
        return seqnum;
    }
}

ST_TYPE void (_set_fire_write_event_cb)(void *_this_v,
                                        user_fire_wevent_func *cb,
                                        user_fire_wevent_func_arg arg) {
    T _this_ = (T) _this_v;
    _this_->fire_write_event = cb;
    _this_->fire_write_event_arg = arg;
}

ST_TYPE int (_stop)(void *_this_v) {
    OE_ILOG( NULL, OEDB_STOPPING_STORE);
    return 0;
}

ST_TYPE int (_stats)(void *_this_v) {
    T _this_ = (T) _this_v;

    int statlvl = DB_STAT_ALL;

    OE_ILOG( NULL, OEDB_LOG_STATS_FOR_ENV);
    DB_ENV *env = _this_->env;
    env->stat_print(env, statlvl);
    OE_ILOG( NULL, OEDB_LOG_STATS_FOR_ID_DB);
    DB *db = _this_->id_db;
    db->stat_print(db, statlvl);
    OE_ILOG( NULL, OEDB_LOG_STATS_FOR_TXN_DB);
    db = _this_->txn_db;
    db->stat_print(db, statlvl);
    OE_ILOG( NULL, OEDB_LOG_STATS_FOR_HOLDER_DB);
    db = _this_->txn_db;
    db->stat_print(db, statlvl);
    OE_ILOG( NULL, OEDB_LOG_STATS_FOR_OBJ_DB);
    db = _this_->txn_db;
    db->stat_print(db, statlvl);
    OE_ILOG( NULL, OEDB_LOG_STATS_FOR_PROP_DB);
    db = _this_->txn_db;
    db->stat_print(db, statlvl);
    OE_ILOG( NULL, OEDB_LOG_STATS_FOR_OWNER_DB);
    db = _this_->txn_db;
    db->stat_print(db, statlvl);
    OE_ILOG( NULL, OEDB_LOG_STATS_FOR_EXPTIME_DB);
    db = _this_->txn_db;
    db->stat_print(db, statlvl);
    return 0;
}

ST_TYPE int (_api_get)(void *_this_v, OeStoreQuery query) {
    T _this_ = (T) _this_v;

    bool take = OeStoreQuery_is_take(query);
    oe_id tid = OeStoreQuery_get_owner_id(query);
    if (take && tid && !_txn_is_active(_this_, tid, OeStoreQuery_get_arena(query))) {
        return _handlerc(OESTORE_INACTIVE_TXN, NULL);
    }

    ///////////////////////////////////
    /// a 0 for max_items_to_return ///
    /// indicates you want and AND  ///
    ///////////////////////////////////
    size_t max_items_to_return = OeStoreQuery_get_max_results(query);
    bool _AND = max_items_to_return == 0;;
    if (_AND) max_items_to_return = 1;

    volatile int ret;

    DB_TXN *txn_holder;
    ret = _this_->env->txn_begin(_this_->env, NULL, &txn_holder, OE_DB_TXN_TYPE);
    if (ret) {
        return _handlerc(ret, NULL);
    }
    txn_holder->set_name(txn_holder, OEDB_TXN_NAME_GET);

    DataObjectList value_lists = OeStoreQuery_get_value_lists(query);
    for (Iterator iter = DataObjectList_iterator(value_lists, true);
        Iterator_hasMore(iter);) {

        size_t hits = OeStoreQuery_get_nitems_found(query);
        DataObject data = Iterator_next(iter);
        ret = _get_one(_this_, query, data, txn_holder);

        if (ret && ret != DB_NOTFOUND) break;   //error
        if (_AND && hits == OeStoreQuery_get_nitems_found(query)) { //a miss is fatal because this is an &&
            ret= DB_NOTFOUND;
            OeStoreQuery_reset_nitems_found(query);
            break;
        }
    }
    size_t items_found = OeStoreQuery_get_nitems_found(query);
    if (!_AND && ret == DB_NOTFOUND
        && items_found) ret = 0; //an or missed one but found others
    if (ret) {
        _finish_txn(txn_holder, false);
        return _handlerc(ret, NULL);
    }

    if (take && items_found > 0) {

        oe_id owner_id = OeStoreQuery_get_owner_id(query);
        //rewrite with locks OR delete if not user txn
        if (owner_id == 0) {
            ret = _delete_items_for_take(_this_, query, txn_holder);
        } else {
            ret = _update_locks_for_take(_this_, query, txn_holder);
        }
        if (ret != 0) {
            _finish_txn(txn_holder, false);
            return _handlerc(ret, NULL);
        }
    }

    //commit txn
    if (take && items_found > 0) {
        int lret = _finish_txn(txn_holder, true);
        return _handlerc(lret, NULL);
    } else {
        _finish_txn(txn_holder, false);
        txn_holder = NULL;
    }

    //in case it bypassed locked ones,
    if (items_found <= 0) {
        return OE_STORE_ITEM_NOTFOUND;
    }

    return _handlerc(ret, txn_holder);
}

ST_TYPE int _update_lease(void *_this_v, oe_id lease_id, oe_time expire_time, Arena_T arena) {
    T _this_ = (T) _this_v;

    volatile int ret;

    //start txn
    DB_TXN *txn;
    ret = _this_->env->txn_begin(_this_->env, NULL, &txn, OE_DB_TXN_TYPE);
    if (ret != 0) {
        OE_ERR(NULL,OEDB_STR_TXN_BEGIN_ERR, db_strerror(ret));
        return _handlerc(ret, NULL);
    }
    txn->set_name(txn, OEDB_TXN_NAME_UPDATE_LEASE);

    DBT key, data;
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));
    data.flags = DB_DBT_MALLOC;

    key.data = &lease_id;
    key.size = sizeof(oe_id);

    ret = _get(arena, _this_->holder_db, txn, &key, &data, DB_RMW);
    if (ret) {
        if (ret == DB_NOTFOUND) {
            /* //ejs todo: leak??
            if (dur <= 0) {
                (void)txn->abort(txn);
                ret = 0;//cancelling an expired lease is a noop
            }
            */
        } else {
            _this_->holder_db->err(_this_->holder_db, ret, OEDB_STR_NOT_GET_H_BY_LID);
        }
        return _handlerc(ret, txn);
    }

    OeStoreHolder rholder = OeStoreHolder_new_from_buffer(arena, data.data);

    oe_time now = oec_get_time_milliseconds();

    oe_time holder_exptime = OeStoreHolder_get_lease_expire_time( rholder);
    if (holder_exptime != OEC_FOREVER && holder_exptime < now) {
        (void)txn->abort(txn);
        return -1;
    }

    //OeStoreHolder_set_lease_expire_time( rholder,  now + dur);
    OeStoreHolder_set_lease_expire_time(rholder,  expire_time);

    char *buffer;
    int buffer_len;
    ret = OeStoreHolder_marshal( rholder, &buffer, &buffer_len, arena );

    data.data = buffer;
    data.size = buffer_len;
    data.flags = DB_DBT_USERMEM;

    ret = _this_->holder_db->put(_this_->holder_db, txn, &key, &data, 0);
    if (ret) {
        _this_->holder_db->err(_this_->holder_db, ret, OEDB_STR_NOT_COMMIT_PUT_HDR);
        return _handlerc(ret, txn);
    }

    if (!(txn->flags & TXN_DEADLOCK)) {
        ret = txn->commit(txn, 0);
        txn = NULL;
    } else {
        ret = DB_LOCK_DEADLOCK;
    }
    if (ret != 0) {
        _this_->holder_db->err(_this_->holder_db, ret, OEDB_STR_NOT_COMMIT_UPL);
    }
    return _handlerc(ret, txn);
}

ST_TYPE int (_api_put)(void *_this_v, OeStoreHolder holder, Arena_T arena) {
    T _this_ = (T) _this_v;

    DB_TXN *txn_holder;
    volatile int ret;
    oe_id lease_id = _create_id(_this_);
    OeStoreHolder_set_lease_id( holder, lease_id );

    DB_ENV *env = _this_->env;
    ret = env->txn_begin(env, NULL, &txn_holder, OE_DB_TXN_TYPE);
    if (!ret) {
        txn_holder->set_name(txn_holder, OEDB_TXN_NAME_PUT);
        ret = _try_put(_this_,holder, arena,txn_holder);
    }
    return _handlerc(ret, txn_holder);
}

//no nested oetxn support... ever?
ST_TYPE int (_start_txn)(void *_this_v, oe_dur dur, OESTORE_TXN *oetxn, Arena_T arena) {
    T _this_ = (T) _this_v;

    assert(arena);

    DB_TXN *txn_holder;

    int ret = _this_->env->txn_begin(_this_->env, NULL, &txn_holder, OE_DB_TXN_TYPE);
    if (ret) {
        OE_ERR(NULL,OEDB_STR_TXN_BEGIN_ERR, db_strerror(ret));
        return _handlerc(ret, NULL);
    }
    txn_holder->set_name(txn_holder, OEDB_TXN_NAME_BEGIN);
    oe_time now = oec_get_time_milliseconds();
    oe_time exptime;
    if (dur == OEC_FOREVER) {
        exptime = OEC_FOREVER;
    } else {
        exptime = now + dur;
    }

    //create txn
    oe_id tid           = _create_id(_this_);
    OE_DLOG(NULL, OEDB_TXN_BEGIN_TIME, tid, dur, now, exptime);
    oetxn->tid          = tid;
    oetxn->begin_time   = now;
    oetxn->end_time     = 0;
    oetxn->expire_time  = exptime;
    oetxn->status       = OESTORE_TXN_ACTIVE;

    ret = _put_oetxn(_this_, oetxn, txn_holder, arena);

    if (!ret) {
        ret = txn_holder->commit(txn_holder, 0);
        txn_holder = NULL;
        return _handlerc(ret, txn_holder);
    }

    return _handlerc(ret, txn_holder);
}

/*
ST_TYPE int (_query_txn)(void *_this_v, oe_id tid, oe_time *expire_time, OESTORE_TXN_STATUS *status, Arena_T arena) {
    T _this_ = (T) _this_v;

    volatile int ret;
    DB_TXN *txn_holder;
    OESTORE_TXN oetxn;
    memset(&oetxn, 0, sizeof(OESTORE_TXN));

    oetxn.tid = tid;

    ret = _this_->env->txn_begin(_this_->env, NULL, &txn_holder, OE_DB_TXN_TYPE);
    if (ret) {
        OE_ERR(NULL,OEDB_STR_TXN_BEGIN_ERR, db_strerror(ret));
        return _handlerc(ret, NULL);
    }
    txn_holder->set_name(txn_holder, OEDB_TXN_NAME_QUERY_TXN);

    ret = _get_txn(_this_, arena, &oetxn, txn_holder);
    if (ret) {
        return _handlerc(ret, txn_holder);
    }

    *expire_time = oetxn.expire_time;
    *status = oetxn.status;
    (void)txn_holder->abort(txn_holder);
    return _handlerc(0, txn_holder);
}
*/

ST_TYPE int (_update_txn)(void *_this_v, oe_id tid, oe_dur dur, OESTORE_TXN_STATUS status, Arena_T arena, int expected_enlisted) {
    T _this_ = (T) _this_v;

    volatile int ret;
    DB_TXN *txn_holder;
    OESTORE_TXN oetxn;
    memset(&oetxn, 0, sizeof(OESTORE_TXN));

    oetxn.tid = tid;

    ret = _this_->env->txn_begin(_this_->env, NULL, &txn_holder, OE_DB_TXN_TYPE);
    if (ret) {
        OE_ERR(NULL,OEDB_STR_TXN_BEGIN_ERR, db_strerror(ret));
        return _handlerc(ret, NULL);
    }
    txn_holder->set_name(txn_holder, OEDB_TXN_NAME_UPDATE_TXN);

    ret = _get_txn(_this_, arena, &oetxn, txn_holder);
    if (ret) {
        return _handlerc(ret, txn_holder);
    }

    //if a txn is already committed, all you can do is commit it
    if (oetxn.status == OESTORE_TXN_ROLLED_BACK) {
        (void)txn_holder->abort(txn_holder);
        OE_DLOG(NULL, OEDB_STR_UPDATING_ROLLEDBACK_TXN);
        if (status != OESTORE_TXN_ROLLED_BACK) {
            return OESTORE_UPDATING_ROLLEDBACK_TXN_ERR;
        }
        return 0;
    }
    //if a txn is already rolled back, all you can do is roll it back
    if (oetxn.status == OESTORE_TXN_COMMITTED) {
        (void)txn_holder->abort(txn_holder);
        OE_DLOG(NULL, OEDB_STR_UPDATING_COMMITTED_TXN);
        if (status != OESTORE_TXN_COMMITTED) {
            return OESTORE_UPDATING_COMMITTED_TXN_ERR;
        }
        return 0;
    }
    //else, if a txn is expired, you can't do anything to it
    oe_time now = oec_get_time_milliseconds();
    OE_DLOG(NULL, OEDB_TXN_UPDATE_TIME, tid, dur, now, oetxn.expire_time);
    if (oetxn.expire_time != OEC_FOREVER &&
        oetxn.expire_time < now) {
        (void)txn_holder->abort(txn_holder);
        OE_DLOG(NULL, OEDB_STR_UPDATING_EXPIRED_TXN);
        return OESTORE_UPDATING_EXPIRED_TXN_ERR;
    }

    if (dur > 0) {
        oetxn.expire_time = now + dur;
    }

    //List_T unlocked_items = List_list(NULL);
    Oec_AList unlocked_items = Oec_AList_list(arena, NULL);
    switch (status) {
    case OESTORE_TXN_COMMITTED:
        OE_DLOG(NULL, "trying to commit\n");
        //try to commit
        ret = _finish_oetxn(_this_, arena, &oetxn, txn_holder,
                            OESTORE_TAKE_LOCK, OESTORE_WRITE_LOCK, status, &unlocked_items, expected_enlisted);
        break;
    case OESTORE_TXN_ROLLED_BACK:
        OE_DLOG(NULL, "trying to rollback\n");
        //try to rollback
        ret = _finish_oetxn(_this_, arena, &oetxn, txn_holder,
                            OESTORE_WRITE_LOCK, OESTORE_TAKE_LOCK, status, &unlocked_items, -1);
        break;
    default:
        break;
    }
    if (ret) {
        return _handlerc(ret, txn_holder);
    }
    ret = _put_oetxn(_this_, &oetxn, txn_holder, arena);
    if (ret) {
        return _handlerc(ret, txn_holder);
    } else {

        if (!(txn_holder->flags & TXN_DEADLOCK)) {
            ret = txn_holder->commit(txn_holder, 0);
            txn_holder = NULL;
        } else {
            ret = DB_LOCK_DEADLOCK;
        }
        if (ret) {
            return _handlerc(ret, txn_holder);
        }
        if ( _this_->fire_write_event && unlocked_items && Oec_AList_length(unlocked_items) > 0 ) {
            DataObject *parray = (DataObject*) Oec_AList_toArray(unlocked_items, arena, NULL);
            for ( int i = 0; parray[i]; i++ ) {
                DataObject data = parray[i];
                _this_->fire_write_event( data, _this_->fire_write_event_arg );
            }
        }
    }

    return _handlerc(ret, txn_holder);
}


bool (_txn_is_active)(T _this_, oe_id tid, Arena_T arena) {

    assert(_this_);
    assert(tid);
    assert(arena);

    volatile int ret;
    OESTORE_TXN oetxn;
    memset(&oetxn, 0, sizeof(OESTORE_TXN));

    oetxn.tid = tid;

    ret = _get_txn(_this_, arena, &oetxn, NULL);
    if (ret) {
        OE_ERR(NULL, OEDB_STR_TXN_IS_NOT_ACTIVE, db_strerror(ret)); //i18n
        return false;
    }

    if (oetxn.status == OESTORE_TXN_ACTIVE) {
        return true;
    } else  {
        return false;
    }
}


ST_TYPE int (_dbfree)(void **_this_p) {

    assert(_this_p && *_this_p);
    T _this_ = (T) *_this_p;

    _set_running(_this_, false);

    if (watchdog_thread_id) {
        oe_pthread_join(watchdog_thread_id, NULL);
        oe_pthread_detach(watchdog_thread_id);
    }

    int ret;
    if (( ret = _this_->seq->close(_this_->seq, 0)) != 0) { //close seq
        _this_->id_db->err(_this_->id_db, ret, OEDB_STR_DB_SEQ_CLOSE_ERR);
    }
    _close_db( _this_->id_db   );
    _close_db( _this_->prop_idx);
    _close_db( _this_->owner_id_idx);
    _close_db( _this_->expire_time_idx);
    _close_db( _this_->holder_db);
    _close_db( _this_->txn_db);

    _this_->env->close(_this_->env, 0);

    Mem_free(_this_, __FILE__, __LINE__);

    return ret;
}

//return -1 for err, 0 for success, 1 for 'more to do'
ST_TYPE int (_lease_reaper)(void *_this_v) {
    T _this_ = (T) _this_v;

    int ret = 0;
    int lret = 0;

    if (_set_reaper_running(_this_, true)) {
        DB_TXN *txn_holder;
        lret = _this_->env->txn_begin(_this_->env, NULL, &txn_holder, 0);
        DBC *cur;

        DBT key, data;
        memset(&key, 0, sizeof(DBT));
        memset(&data, 0, sizeof(DBT));
        data.flags = DB_DBT_MALLOC;
        oe_time now = oec_get_time_milliseconds();

        key.data = &now;
        key.size = sizeof(oe_time);
        DB *idx = _this_->expire_time_idx;
        if ((lret = idx->cursor(idx, txn_holder, &cur, OE_DB_TXN_TYPE )) != 0) {
            txn_holder->abort(txn_holder);
            _set_reaper_running(_this_, false);
            OE_ERR(NULL,OEDB_STR_LSE_RPR_CUR_ERR, db_strerror(lret));
            return -1;
        }

        int cnt = 0;
        for ( lret = _get_first(cur, &key, &data); lret == 0; lret = _get_prev(cur, &key, &data ) ) {
            cnt++;
            oe_id owner = OeStoreHolder_peek_txn_id(data.data);
            assert(!owner);
            oe_time exptime = OeStoreHolder_peek_lease_exp_time(data.data);
            if (exptime > now) {
                continue;
            }
#ifdef OE_DEBUG
            oec_print_timestamp(stdout);
            char *expstr;
            oec_print_timestamp_to_str(NULL, &expstr, exptime);
            OE_DLOG(NULL,OEDB_STR_LSE_RPR_ITEM, cnt, OeStoreHolder_peek_lease_id(data.data), expstr);
            Mem_free(expstr, __FILE__, __LINE__);
#endif
#ifdef OE_TRACE
            oec_print_hex_and_ascii( data.data, data.size );
#endif

            assert(exptime <= now);
            int llret = cur->del(cur, 0);
            Mem_free( data.data, __FILE__, __LINE__ );
            if (llret && llret != DB_NOTFOUND) {
                cur->close(cur);
                txn_holder->abort(txn_holder);
                _set_reaper_running(_this_, false);
                OE_ERR(NULL,OEDB_STR_LSE_RPR_DEL_ERR, db_strerror(llret));
                return -1;
            }
            if (cnt >= OESTORE_MAX_LEASE_REAP) {
                ret = 0;
                break;
            }
            if (cnt >= OESTORE_MAX_LEASE_REAP_COMMIT) {
                ret = 1;
                break;
            }
        }
        if (lret && lret != DB_NOTFOUND) {
            _set_reaper_running(_this_, false);
            cur->close(cur);
            txn_holder->abort(txn_holder);
            OE_ERR(NULL,OEDB_STR_LSE_RPR_ERR, db_strerror(lret));
            return -1;
        }
        lret = cur->close(cur);
        if (lret) {
            _set_reaper_running(_this_, false);
            txn_holder->abort(txn_holder);
            return -1;
        }
        lret = txn_holder->commit(txn_holder, 0);
        txn_holder = NULL;
        if (lret) {
            _set_reaper_running(_this_, false);
            txn_holder->abort(txn_holder);
            OE_ERR(NULL,OEDB_STR_LSE_RPR_COMMIT_ERR, db_strerror(lret));
            return -1;
        }

        //process all expired

        _set_reaper_running(_this_, false);
    }
    return ret;
}

ST_TYPE void _start_deadlock_watchdog(void *_this_v, bool threaded) {
    T _this_ = (T) _this_v;

    if ( !threaded ) {
        OE_ILOG(NULL, OEDB_STR_CONFIG_DEADLOCK_WATCHDOG);
        _this_->env->set_lk_detect(_this_->env, DB_LOCK_YOUNGEST);
    } else {
        OE_ILOG(NULL, OEDB_STR_START_DL_WATCHDOG);
        pthread_attr_t attr;
        oe_pthread_attr_init(&attr);
        int ret = oe_pthread_create( &watchdog_thread_id, &attr, _deadlock_watchdog, _this_);
        if (ret) {
            OE_ERR(NULL, OEDB_STR_CAN_NOT_CREATE_DL_WDOG);
            assert(0);
        }
    }
}

ST_TYPE char *_strerror(void * _this_v, int ret) {

    switch (ret) {
    case OE_STORE_PLEASE_RETRY:
        return OEDB_STR_DEADLOCK_PLEASE_RETRY;
    default:
        return db_strerror(ret);
    }
}

ST_TYPE int _new(T *_this_p, int persist_level, const char *homedir, bool threaded) {

#ifdef OE_NOTHREADS
    threaded = false;
#endif

    T _this_;

    _this_ = Mem_alloc(sizeof *_this_, __FILE__, __LINE__);
    _this_->fire_write_event = NULL;
    _this_->db_home_dir     = homedir;
    _this_->persist_level   = persist_level;

    DB_ENV *env;

    int ret = db_env_create( &env, 0 );
    env->set_errcall( env, &_log_err );
    env->set_msgcall( env, &_log_msg );
    env->set_timeout( env, (u_int32_t) 400000, DB_SET_LOCK_TIMEOUT ); //todo: config
    env->set_timeout( env, (u_int32_t) 900000, DB_SET_TXN_TIMEOUT ); //todo: config
    FILE *f = fopen("db_error.log", "w");
    env->set_errfile(env, f);

    if (ret != 0) {
        OE_ERR(NULL, OEDB_STR_ERR_CREATING_ENV, db_strerror(ret));
        return -1;
    }

    if (persist_level <= 1) {
        //probably needs to query the environment to pick a size
        ret = env->set_cachesize( env, 1, 0, 10 );  //1 gig, 0 meg, 1 contig space todo: config
        if (ret != 0) {
            OE_ERR(NULL, OEDB_STR_ERR_CREATING_ENV_CACHE, db_strerror(ret));
            return -1;
        }
        ret = env->set_cache_max(env, 1, 0);
        u_int32_t gbytes;
        u_int32_t nbytes;
        int ncache;
        //int rc = env->get_cachesize(env, &gbytes, &nbytes, &ncache );
        //printf("cache %ld %ld %ld\n", gbytes, nbytes, ncache);

        //this call is not there until 4.7 and the reason for our 4.7 requirement.
        ret = env->log_set_config(env, DB_LOG_IN_MEMORY, 1);

        ret = env->set_lg_bsize( env, 10 * 1024 * 1024 );//100 meg log?
        assert( !ret );
    }
    //env->set_lk_max_locks(env, 50000);
    //env->set_lk_max_lockers(env, 50000);
    //env->set_lk_max_objects(env, 50000);
    env->set_tx_max(env, 2000); //todo: config

    //open environment
    _this_->env = env;

    u_int32_t env_flags = 
    //DB_RECOVER      | //todo: let unit tests turn this off or get smart enough to let it run
    DB_INIT_LOG     |
    DB_INIT_LOCK    |
    DB_INIT_TXN     |
//#ifdef OE_USE_THREADS
    DB_THREAD       |  //don't seem to be able get rid of this in single t mode.  something isn't doing userme
//#endif
    DB_INIT_MPOOL   |
    DB_CREATE;

    if (persist_level > 1) {
        ret = env->open(env, _this_->db_home_dir, env_flags, 0);
    } else {
        env_flags = env_flags | DB_PRIVATE;
        ret = env->open(env, NULL, env_flags, 0);
    }
    if (ret != 0) {
        OE_ERR(NULL,OEDB_STR_ERR_OPENING_ENV, db_strerror(ret));
        return ret;
    }

    if (_this_->persist_level == OESTORE_PERSIST_NO_SYNC) {
        env->set_flags( env, DB_TXN_NOSYNC, 1 );
    }

    //open id generator database and sequence
    DB_TXN *txn;
    ret = _this_->env->txn_begin(_this_->env, NULL, &txn, 0);
    if (ret != 0) {
        OE_ERR(NULL,OEDB_STR_NOT_BEGIN_DB_OPEN);
        return ret;
    }
    txn->set_name(txn, OEDB_TXN_NAME_OPEN_ENV);

    //open primary id db
    ret = _open_database(_this_, txn, &_this_->id_db, OEDB_STR_ID_DB_FILENAME, false, true, NULL, false);
    if (ret != 0) {
        return ret;
    }
    DBT key;
    if ((ret = db_sequence_create(&_this_->seq, _this_->id_db, 0)) != 0) {
        OE_ERR(NULL, OEDB_STR_SEQ_CREATE_ERR);
        return ret;
    }
    _this_->seq->set_cachesize(_this_->seq, 100000); //todo: make config
    _this_->seq->initial_value(_this_->seq, 1001);

    memset(&key, 0, sizeof(DBT));
    char *SEQ_NAME = "id.seq";
    key.data = SEQ_NAME;
    key.size = (u_int32_t)strlen(SEQ_NAME);
    //key.flags = DB_DBT_APPMALLOC;

    //if ((ret = _this_->seq->open(_this_->seq, NULL, &key, DB_CREATE)) != 0) {
    //if ((ret = _this_->seq->open(_this_->seq, NULL, &key, DB_CREATE | DB_THREAD)) != 0) {
    if ((ret = _this_->seq->open(_this_->seq, txn, &key, DB_CREATE | DB_THREAD)) != 0) {
        OE_ERR(NULL,OEDB_STR_SEQ_OPEN_ERR);
        return ret;
    }

    //open user txn databases

    //open primary txn db
    ret = _open_database( _this_, txn, &_this_->txn_db, OEDB_STR_TXN_DB_FILENAME, false, true, NULL, false );
    if (ret) {
        return ret;
    }

    //open primary holder db
    ret = _open_database( _this_, txn, &_this_->holder_db, OEDB_STR_HOLDER_DB_FILENAME, false, true, NULL, false );
    if (ret) {
        return ret;
    }

    //open expire time index (items have leases and leases expire)
    ret = _open_database( _this_, txn, &_this_->expire_time_idx, OEDB_STR_EXP_IDX_FILENAME, true, true, _compare_oe_time, true );
    if (ret) {
        return ret;
    }

    //open owner id secondary databases (transactions own records)
    ret = _open_database( _this_, txn, &_this_->owner_id_idx, OEDB_STR_OWNER_IDX_FILENAME, true, true, _compare_oe_id, false );
    if (ret) {
        return ret;
    }

    //open props secondary databases (tuple fields)
    ret = _open_database( _this_, txn, &_this_->prop_idx, OEDB_STR_PROP_IDX_FILENAME, true, true, NULL, false );
    if (ret) {
        return ret;
    }
    ret = txn->commit(txn, 0);
    txn = NULL;
    if (ret) {
        OE_ERR(NULL,OEDB_STR_OPEN_TXN_FAILED);
        return ret;
    }

    _this_->holder_db->associate(_this_->holder_db, NULL, _this_->prop_idx, &_set_props, 0);
    _this_->holder_db->associate(_this_->holder_db, NULL, _this_->expire_time_idx, &_set_expire_time, 0);
    _this_->holder_db->associate(_this_->holder_db, NULL, _this_->owner_id_idx, &_set_owner, 0);

    *_this_p = _this_;

    init_running_flag(_this_);
    _set_running(_this_, true);
    _start_deadlock_watchdog(_this_, threaded);

    _init_reaper_running_flag(_this_);

    return 0;
}

OeStore (OeBdb_new)(int persist_level, const char *homedir, bool threaded) {

    T _this_ = NULL;
    _new(&_this_, persist_level, homedir, threaded);
    assert(_this_);

    OeStore store = OeStore_new(_this_,
                                _strerror,
                                _stop,
                                _dbfree,
                                _stats,
                                _deadlock_detect,
                                _start_deadlock_watchdog,
                                _set_fire_write_event_cb,
                                _lease_reaper,
                                _create_id,
                                _api_put,
                                _api_get,
                                _start_txn,
                                _update_txn,
                                //_query_txn,
                                _update_lease);


    return store;
}

#undef T
#undef HT

