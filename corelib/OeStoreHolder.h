/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OESTOREHOLDER_H
#define OESTOREHOLDER_H

#include "oe_common.h"
#include "DataObject.h"
#include <stdbool.h>

enum _oestore_lock {OESTORE_UNLOCKED, OESTORE_WRITE_LOCK, OESTORE_TAKE_LOCK};
typedef enum _oestore_lock OESTORE_LOCK; 

enum _oestore_skip_status {OESTORE_DO_NOT_SKIP, OESTORE_SKIP, OESTORE_SKIP_BUT_EXISTS};
typedef enum _oestore_skip_status OESTORE_SKIP_STATUS ; 

#define T OeStoreHolder
struct store_holder_T {

    OESTORE_LOCK lock;

    bool      is_tuple;
    oe_id     lease_id;
    oe_id     txn_id;
    oe_time   lease_expire_time;

    Arena_T arena;
    DataObject data;
};
typedef struct store_holder_T *T;

OESTORE_SKIP_STATUS OeStoreHolder_skip(T);
extern T OeStoreHolder_new(Arena_T arena);
extern T OeStoreHolder_new_from_buffer(Arena_T arena, char *);
extern T OeStoreHolder_new_with_data(Arena_T, DataObject);

// getters and setters
extern bool OeStoreHolder_is_tuple(T);
extern void OeStoreHolder_set_tuple(T,  bool);

extern void     OeStoreHolder_set_lock             (T, OESTORE_LOCK);
extern OESTORE_LOCK OeStoreHolder_get_lock         (T);

extern void     OeStoreHolder_set_txn_id           (T, oe_id);
extern oe_id    OeStoreHolder_get_txn_id           (T);

extern void     OeStoreHolder_set_lease_expire_time(T, oe_time);
extern oe_time  OeStoreHolder_get_lease_expire_time(T);

extern DataObject OeStoreHolder_get_data            (T);
extern void OeStoreHolder_set_data(T, DataObject);

extern void     OeStoreHolder_set_lease_id         (T, oe_id);
extern oe_id    OeStoreHolder_get_lease_id         (T);

extern int      OeStoreHolder_marshal              (T, char **, int *, Arena_T);
extern oe_id    OeStoreHolder_peek_txn_id          (char *);
extern oe_time  OeStoreHolder_peek_lease_exp_time  (char *);
extern oe_id    OeStoreHolder_peek_lease_id        (char *);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

