/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEK_IMPL_H
#define OEK_IMPL_H
#include "OEK.h"
#include "Oed_Dispatcher.h"
#include "Oe_Thread_Dispatcher.h"
#include "OeStore.h"

#define T OEK

typedef struct Oek_Impl_ {

    Oed_Dispatcher template_matcher;
    Oe_Thread_Dispatcher db_dispatcher;
    OeStore store;

} Oek_Impl_;

#define OEK_MAX_DB_DEADLOCKS         10000
#define OEK_DEFAULT_PERSIST_LEVEL    3

void (_oek_impl_put) (T, OEK_Context *);
void (_oek_impl_get) (T, OEK_Context *);
void (_oek_impl_sta) (T, OEK_Context *);
void (_oek_impl_upt) (T, OEK_Context *);
void (_oek_impl_upl) (T, OEK_Context *);
void _oek_impl_free(T *_this_p);

Oed_Dispatcher       _get_matcher      (OEK);
Oe_Thread_Dispatcher _get_db_dispatcher(OEK);
OeStore              _get_store        (OEK);

void _send_error(OEK_Context *, oe_id, char *);
void _handle_mem_error(OEK_Context *context);
void _default_write_to_client(OEK_Context *context);

extern T (OEK_default_factory_new)(Oed_Dispatcher, Oe_Thread_Dispatcher, OeStore);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

