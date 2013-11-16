/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
///                                                   ///
/// INTEGRATION: if you are looking at this file it   ///
/// may be that you want to write a server module.    ///
///                                                   ///
/// Please consider writing the module using either   ///
/// the lua oelapi api or the lua oescript api.  If   ///
/// extending the server with the native api is       ///
/// really what you want, see the product source      ///
/// and look in the modules sub project for examples. ///
/// See OESP for a protocol implementation and        ///
/// evalmod for a service implementation.             ///
///                                                   ///
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

#ifndef OEK_H
#define OEK_H

#include "set.h"
#include "arena.h"
#include "oec_values.h"
#include "Oec_AList.h"
#include "OekMsg.h"

struct oek_T;

typedef struct OEK_Context {

    Arena_T arena;
    oe_time start_time;
    oe_time expire_time;

    //input - protocol sets these before each kernel cmd call
    OekMsg cmd_msg;
    void    (*write_response_cb) (struct OEK_Context *); //callback for all kernel cmd calls

    //output - kernel sets these before calling the protocol callback
    Oec_AList res_msgs;
    OekMsg err_msg;

    //client modules can use this field for state, connection holders, etc...
    void *p_context;
    struct oek_T *kernel;

} OEK_Context;

typedef void OEK_kernel_call(struct oek_T *);
typedef void OEK_kernel_ctx_call(struct oek_T *, OEK_Context *);
typedef void OEK_kernel_free_call(struct oek_T**);

#define T OEK
struct oek_T {

    void                 *_client;
    OEK_kernel_ctx_call  *_put;
    OEK_kernel_ctx_call  *_get;
    OEK_kernel_ctx_call  *_sta;
    OEK_kernel_ctx_call  *_upt;
    OEK_kernel_ctx_call  *_upl;
    OEK_kernel_free_call *_free;
    OEK_kernel_call      *_stop;
    OEK_kernel_call      *_stats;
};

typedef struct oek_T *T;

#define	OEK_CONTEXT_ROLLBACK_ONLY  0x0000001

extern void (OEK_Context_free)( OEK_Context **);
extern OEK_Context *(OEK_Context_new)(void);

extern oe_id (OEK_create_id)( T );

extern T    (OEK_new) (void *_client,
                       OEK_kernel_ctx_call *_oek_impl_put,
                       OEK_kernel_ctx_call *_oek_impl_get,
                       OEK_kernel_ctx_call *_oek_impl_sta,
                       OEK_kernel_ctx_call *_oek_impl_upt,
                       OEK_kernel_ctx_call *_oek_impl_upl,
                       OEK_kernel_free_call *_oek_impl_free,
                       OEK_kernel_call *_stop,
                       OEK_kernel_call *_stats);
extern void *OEK_get_client(T);
extern void (OEK_free)(T *);
extern void (OEK_stop)(T);
extern void (OEK_stats)(T);
// 's___' functions are sync, all others are async and you better
// set the 'write_response_cb' func in context
extern void (OEK_put) (T, OEK_Context *);
extern void (OEK_get) (T, OEK_Context *);
extern void (OEK_sta) (T, OEK_Context *);
extern void (OEK_upt) (T, OEK_Context *);
extern void (OEK_upl) (T, OEK_Context *);
extern void (OEK_exec)(T, OEK_Context *); //todo: stdize on this, hide _put, _get, etc...

#undef T

#endif

#ifdef __cplusplus
}
#endif 

