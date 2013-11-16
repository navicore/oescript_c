/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OESESSION_H
#define OESESSION_H

#include "config.h"
#include "set.h"
#include <stdbool.h>
#include "oec_values.h"
#include "OeNetConn.h"
#include "OeNet.h"

struct session_T;

#define T OeSession
struct session_T {

    oe_id               sid;
    oe_time             start_time;
    //OeNet               net;
    OeNet               net;
    Arena_T             arena;
    OeNetConn           conn;
    oe_flags            state;
    void                *sess_context;  //duration of session
    void                *appl_context;  //duration of txn (used only for partial reads)
    void                *proto_context; //duration of server
    void                *subject;
    bool                rb_on_dis;
    //note, this goes negative for clients and counts up to zero
    int                 outstanding_task_count;
    Set_T               pending_txns;
};

typedef struct session_T *T;

#define	OENET_CLIENT_CLOSING           0x0000002
#define	OENET_CLIENT_CONNECTED         0x0000004

extern Arena_T OeSession_get_arena(T);

extern void *OeSession_get_net(T);
extern OeNetConn OeSession_get_conn(T);

extern T oesession_new_dummy();
extern T oesession_new(void *);
extern void OeSession_free(T*);

extern bool OeSession_state_is_set(T, oe_flags);
extern void OeSession_state_set(T, oe_flags);
extern void OeSession_state_clear(T, oe_flags);

//per txn data for the kernel context (rarely set except during partial reads)
extern void OeSession_set_appl_context(T , void *);
extern void *OeSession_get_appl_context(T);

//for duration of session - maybe hold connection obj once authenticated
extern void OeSession_set_sess_context(T , void *);
extern void *OeSession_get_sess_context(T);

//for the protocol impl
extern void OeSession_set_protocol_context( T, void * );
extern void *OeSession_get_protocol_context( T );

//deperecated?
extern void OeSession_set_username(T, oe_scalar);
extern oe_scalar OeSession_get_username(T);

extern void OeSession_set_username(T, oe_scalar);
extern oe_scalar OeSession_get_username(T);

extern void OeSession_set_id(T, oe_id);
extern oe_id OeSession_get_id(T);

extern int  OeSession_incr_task_count(T);
extern int  OeSession_decr_task_count(T);
extern int  OeSession_get_task_count(T);

//
//you can store a set of oe_ids in the connection and when you see it is
// marked for rollback only, it is an oppertunity to clean up whatever is associated
// with those ids
//
extern Set_T   OeSession_get_tids(T);
extern void    OeSession_save_tid(T, oe_id);
extern oe_id   OeSession_clear_tid(T, oe_id);
extern oe_time OeSession_get_starttime(T);
extern void OeSession_set_rb_on_disconnect(T);
extern bool OeSession_rb_on_disconnect(T);

void OeSession_set_subject(T _this_, void *subject);
void *OeSession_get_subject(T _this_);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

