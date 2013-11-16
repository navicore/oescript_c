/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef Oep_H
#define Oep_H

#include "OEK.h"
#include "oec_values.h"
#include "OepClient.h"
#include "Oed_Dispatcher.h"
#include "OeNet.h"
#include "SecurityContext.h"
#include "LoginModule.h"

#define T OepServer
struct server_T {

    Table_T kernels;
    SecurityContext security_context;
    Oed_Dispatcher dispatcher;
    OeNet net;

    oep_client_read_msg  *read_fun;
    oep_client_write_msg *write_fun;
};

typedef struct server_T *T;

extern T OepServer_new(Oed_Dispatcher, OeNet);

extern int OepServer_listen(T _this_, char *hostifc, int port);

extern void OepServer_free(T*);
extern void OepServer_stats(T);

extern void OepServer_add_login_module(T, LoginModule);
extern void OepServer_add_kernel(T, oe_scalar, OEK);

extern void OepServer_configure(T _this_,
                         oep_client_read_msg  read_fun,
                         oep_client_write_msg write_fun, 
                         oe_id (* idfun) (void *),
                         void *idfun_arg );

#undef T

#endif

#ifdef __cplusplus
}
#endif 

