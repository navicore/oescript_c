/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OES_OESCRIPT_H
#define OES_OESCRIPT_H

#include "DataObject.h"
#include "DataObjectList.h"
#include "SignalHandler.h"
#include "Oed_Dispatcher.h"
#include "Oe_Thread_Dispatcher.h"
#include "OeNet.h"
#include "OeNetDefaultFactory.h"
#include "OeStore.h"
#include "OeStoreDefaultFactory.h"
#include "OEK.h"
#include "JsonKernel.h"
#include "OepFactory.h"
#include "OepServer.h"
#include "OepClient.h"
#include "arena.h"
#include "oe_common.h"
#include "Oec_Future.h"
#include "Oec_CountdownLatch.h"
#include "Iterator.h"
#include "LoginModule.h"
#include "BuiltInLoginModule.h"
#include "PamLoginModule.h"

struct OesKernel;
struct OesJsonKernel;
struct OesTxn;
struct OesLease;
struct OesDataObject;
struct OesDataObjectList;
struct OesIterator;
struct OesLoginModule;
struct OesBuiltInLoginModule;

// results

enum OES_TXN_STATUS {
    ACTIVE,
    ROLLEDBACK,
    COMMITTED
};

typedef void OesKernel_shutdown_cb(void *);
typedef struct {
    OEK obj;
    oe_id lastid;
    struct OesTxn *currentTxn;
    bool managed;
    OesKernel_shutdown_cb *shutdown_cb;
    void *shutdown_cb_arg;
} OesKernel;

typedef struct OesJsonKernel {
    JsonKernel obj;
    bool managed;
} OesJsonKernel;

typedef struct OesTxn {
    //struct OesKernel *kernel_holder;
    OesKernel *kernel_holder;
    Arena_T arena;
    Oec_Future f;
    enum OES_TXN_STATUS status;
    oe_id tid;
    Oec_CountdownLatch enlisted;
    Oec_CountdownLatch enlisted_counter;
    int enlisted_items;
} OesTxn;

typedef struct OesLease {
    //struct OesKernel *kernel_holder;
    OesKernel *kernel_holder;
    Arena_T arena;
    Oec_Future f;
    oe_id lid;
    struct OesTxn *txn_holder;
} OesLease;

typedef struct OesDataObjectList {
    Arena_T arena;
    //struct OesKernel *kernel_holder;
    OesKernel *kernel_holder;
    Oec_Future f;
    DataObjectList obj;
    struct OesTxn *txn_holder;
} OesDataObjectList;

typedef struct {
    Iterator obj;
} OesIterator;

typedef struct {
    Oec_Future f;
    DataObjectList l;
    Iterator obj;
} OesListIterator;

// other objects

typedef struct {
    Arena_T arena;
    DataObject obj;
    bool free_arena;
} OesDataObject;

typedef struct {
    SignalHandler obj;
} OesSigHandler;

typedef struct {
    Oed_Dispatcher obj;
    bool managed;
} OesDispatcher;

typedef struct {
    Oe_Thread_Dispatcher obj;
    bool managed;
} OesThreadDispatcher;

typedef struct {
    Oec_Future obj;
} OesFuture;

typedef struct {
    OeStore obj;
    bool managed;
} OesStore;

typedef struct {
    OeNet obj;
    bool managed;
} OesNet;

typedef struct {
    LoginModule obj;
    bool managed;
} OesLoginModule;

typedef struct {
    BuiltInLoginModule obj;
    bool managed;
} OesBuiltInLoginModule;

typedef struct {
    PamLoginModule obj;
    bool managed;
} OesPamLoginModule;

typedef struct {
    OepServer obj;
    bool managed;
} OesServer;

typedef struct {
    OepClient obj;
    bool managed;
} OesClient;

extern OesTxn *OesKernel_impl_begin(OesKernel *, oe_time);

extern OesLease *OesKernel_impl_write(OesKernel *,
                                      OesDataObject *, oe_time, bool);

extern OesDataObjectList *OesKernel_impl_read (OesKernel *,
                                               OesDataObjectList *,
                                               oe_time, bool, int,
                                               bool, bool);

extern OesDataObjectList *OesKernel_impl_take (OesKernel *,
                                               OesDataObjectList *,
                                               oe_time, bool, int,
                                               bool, bool);

extern void OesKernel_impl_json_promise(char *, void *);

extern OesDataObjectList *OesKernel_impl_createOesDataObjectList(Arena_T,
                                                                 Oec_Future,
                                                                 OesTxn *);

extern void OesKernel_impl_commit(OesTxn *);

extern void OesKernel_impl_rollback(OesTxn *);

extern void OesKernel_impl_upl_cancel(OesLease *);

static char *PARSE_PRIV_ERR = "can not parse priv spec: "; //i18n
static char *PARSE_PRIV_PAM_ERR = "can not parse pam priv spec: "; //i18n
//
//exceptions
//
void throw_exception(oe_scalar);

void clear_exception();

oe_scalar check_exception();

#endif

#ifdef __cplusplus
}
#endif 

