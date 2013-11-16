/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OESTORE_H
#define OESTORE_H

#include <stdio.h>
#include "DataObject.h"
#include "DataObjectList.h"
#include <stdbool.h>
#include "oe_common.h"
#include "OeStoreHolder.h"
#include "OeStoreQuery.h"

///////////////////////////////////
// DB                            //
///////////////////////////////////

enum _oestore_status {
    OESTORE_TXN_COMMITTED, OESTORE_TXN_ROLLED_BACK, OESTORE_TXN_ACTIVE
};
typedef enum _oestore_status OESTORE_TXN_STATUS; 

typedef struct _oestore_txn {

    OESTORE_TXN_STATUS status; 

    oe_time expire_time;
    oe_time begin_time;
    oe_time end_time;

    oe_id   tid; //transient field for gc processing.  do not use outside of space impl

} OESTORE_TXN;

//todo: resource bundles
#define OESTORE_UPDATING_EXPIRED_TXN_ERR       -77000
#define OESTORE_IS_NULL_ERR                    -77001
#define OESTORE_ALREADY_COMPLETE_ERR           -77002
#define OESTORE_UPDATING_ROLLEDBACK_TXN_ERR    -77003
#define OESTORE_UPDATING_COMMITTED_TXN_ERR     -77004
#define OESTORE_ENLISTED_COUNT_INCORRECT       -77005
/* a get or a put was enlisted in a bogus txn */
#define	OESTORE_INACTIVE_TXN                   -78006 

//todo:
//////////////////////////////////////////////////////////////////////
///////////// BDB stuff: GET THIS OUT OF HERE ////////////////////////
///////////// replace with kernel rc codes    ////////////////////////
//////////////////////////////////////////////////////////////////////
#define	OE_STORE_PLEASE_RETRY  (-78000)/* Deadlock. */
#define	OE_STORE_ITEM_NOTFOUND (-78001)/* Key/data pair not found (EOF). */
#define	OE_STORE_MISSING_ATTRS (-78002)/* need keys to store stuff  */
/* if storing a map, an even number of attrs will be turned into key/value keys*/
#define	OE_STORE_ATTRS_NOT_PAIRED (-78003)

#define	__OESTORE_NULL__ "_"
#define	__OESTORE_WILDCARD__ "_"

typedef void user_fire_wevent_func( DataObject, void * );
typedef void *user_fire_wevent_func_arg;

#define OESTORE_PERSIST_SYNC 3
#define OESTORE_PERSIST_NO_SYNC      2
#define OESTORE_PERSIST_NONE         1

#define OESTORE_MAX_LEASE_REAP       10000000
#define OESTORE_MAX_LEASE_REAP_COMMIT 100

typedef char *  OESTORE_strerror                (void *, int);
typedef int     OESTORE_stop                    (void *);
typedef int     OESTORE_free                    (void * *);
typedef int     OESTORE_stats                   (void *);
typedef int     OESTORE_deadlock_detect         (void *, int);
typedef void    OESTORE_start_deadlock_watchdog (void *, bool);
typedef void    OESTORE_set_fire_write_event_cb (void *, user_fire_wevent_func *, user_fire_wevent_func_arg);
typedef int     OESTORE_lease_reaper            (void *);
typedef oe_id   OESTORE_create_id               (void *);
typedef int     OESTORE_put                     (void *, OeStoreHolder, Arena_T);
typedef int     OESTORE_get                     (void *, OeStoreQuery);
typedef int     OESTORE_start_txn               (void *, oe_dur, OESTORE_TXN *, Arena_T);
typedef int     OESTORE_update_txn              (void *, oe_id, oe_dur, OESTORE_TXN_STATUS, Arena_T, int);
//typedef int     OESTORE_query_txn               (void *, oe_id, oe_time *, OESTORE_TXN_STATUS *, Arena_T);
typedef int     OESTORE_update_lease            (void *, oe_id, oe_time, Arena_T);
typedef int     OESTORE_new                     (void * *, int , const char *, bool);

#define T OeStore
struct store_T {
    void *store_impl;
    OESTORE_strerror                *_strerror;
    OESTORE_stop                    *_stop;
    OESTORE_free                    *_oestore_free;
    OESTORE_stats                   *_stats;
    OESTORE_deadlock_detect         *_deadlock_detect;
    OESTORE_start_deadlock_watchdog *_start_deadlock_watchdog;
    OESTORE_set_fire_write_event_cb *_set_fire_write_event_cb;
    OESTORE_lease_reaper            *_lease_reaper; 
    OESTORE_create_id               *_create_id;
    OESTORE_put                     *_oestore_put;
    OESTORE_get                     *_oestore_get; 
    OESTORE_start_txn               *_start_txn;
    OESTORE_update_txn              *_update_txn;
//    OESTORE_query_txn               *_query_txn;
    OESTORE_update_lease            *_update_lease;
    OESTORE_new                     *_new;
};
typedef struct store_T *T;

///////////////////////////////////
// TXN                           //
///////////////////////////////////

extern char *(OeStore_strerror)     (T, int);
extern int (OeStore_stop)           (T);
extern int (OeStore_free)           (T*);
extern int (OeStore_stats)          (T);
extern int (OeStore_deadlock_detect)(T, int);

extern void  (OeStore_start_deadlock_watchdog)(T, bool);
extern void  (OeStore_set_fire_write_event_cb)(OeStore, user_fire_wevent_func *, user_fire_wevent_func_arg);
extern int   (OeStore_lease_reaper)(T);
extern oe_id (OeStore_create_id)   (T);

extern int (OeStore_put)         (T, OeStoreHolder, Arena_T);
extern int (OeStore_get)(T _this_, OeStoreQuery query);

extern int (OeStore_start_txn)   (T, oe_dur, OESTORE_TXN *, Arena_T);
extern int (OeStore_update_txn)  (T, oe_id, oe_time, OESTORE_TXN_STATUS, Arena_T, int);
//extern int (OeStore_query_txn)   (T, oe_id, oe_time *, OESTORE_TXN_STATUS *, Arena_T);
extern int (OeStore_update_lease)(T, oe_id, oe_time, Arena_T);

extern T (OeStore_new)(void *,
                       OESTORE_strerror                *,
                       OESTORE_stop                    *,
                       OESTORE_free                    *,
                       OESTORE_stats                   *,
                       OESTORE_deadlock_detect         *,
                       OESTORE_start_deadlock_watchdog *,
                       OESTORE_set_fire_write_event_cb *,
                       OESTORE_lease_reaper            *,
                       OESTORE_create_id               *,
                       OESTORE_put                     *,
                       OESTORE_get                     *, 
                       OESTORE_start_txn               *,
                       OESTORE_update_txn              *,
//                       OESTORE_query_txn               *,
                       OESTORE_update_lease            *);
#undef T

#endif


#ifdef __cplusplus
}
#endif 

