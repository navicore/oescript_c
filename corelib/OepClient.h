/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEPCLIENT_H
#define OEPCLIENT_H

#include "Oed_Dispatcher.h"
#include "Subject.h"
#include "OeSession.h"
#include "OeNet.h"
#include "OEK.h"
#include "arena.h"
#include "oec_values.h"

typedef int oep_client_peek_msg (OeSession, oe_id*);
typedef int oep_client_read_msg (OeSession, OekMsg*, Arena_T);
typedef int oep_client_write_msg(OeSession, OekMsg, Arena_T);

#define T OepClient
struct client_T {

    OeNet           net;
    Subject         subject;
    char            *spec;
    Oed_Dispatcher  dispatcher;
    oe_id           sid;
    OeSession       session;
    Arena_T         arena;
    Table_T         pending;
    char           *hostname;
    int             port;

    oep_client_peek_msg  *peek_cid_fun;
    oep_client_read_msg  *read_fun;
    oep_client_write_msg *write_fun;
};

typedef struct client_T *T;

extern void OepClient_configure(T _this_,
                                  oep_client_peek_msg,
                                  oep_client_read_msg,
                                  oep_client_write_msg);

extern T OepClient_new(Oed_Dispatcher, OeNet, oe_scalar, oe_scalar, char *);

extern void OepClient_free(T *);

extern void OepClient_stop(T);
extern void OepClient_stats(T);

extern void OepClient_send(OEK, OEK_Context *);

extern int OepClient_connect(T);

extern OEK OepClient_new_kernel(T);
extern void OepClient_kfree(OEK *);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

