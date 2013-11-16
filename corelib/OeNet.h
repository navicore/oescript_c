/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OENET_H
#define OENET_H

//#include "OeSession.h"
#include "oe_common.h"
#include "Oec_IdSet.h"
#include "OeNetConn.h"

struct net_T;

#define T OeNet
typedef void        OENET_listen_for_connect   (struct net_T*, char *, int);
typedef void       *OENET_connect              (struct net_T*, const char *, int);
typedef OeNetConn   OENET_new_conn             (struct net_T*);
//typedef void        OENET_cleanup_conn         (OeSession);
typedef void        OENET_cleanup_conn         (void *);
//typedef OeSession   OENET_listen_for_bytes     (struct net_T*, void *);
typedef void *   OENET_listen_for_bytes     (struct net_T*, void *);

//typedef void oenet_callback(OeSession);
typedef void oenet_callback(void *);

struct net_T {
    oe_scalar                   name; //nice for logging
    OENET_listen_for_connect   *listen_fun;
    OENET_cleanup_conn         *cleanup_conn_fun;
    OENET_connect              *connect_fun; 
    OENET_listen_for_bytes     *listen_for_bytes_fun;  
    OENET_new_conn             *new_conn_fun;

    oenet_callback *read_handler;
    oenet_callback *con_timeout_cb;

    void *net_obj;
    void *proto;

    oe_id( * create_id ) ( void * );
    void *create_id_arg;
    oe_id lastid;
    Oec_IdSet sessions;
};

typedef struct net_T *T;

extern T OeNet_new(void                       *proto,
                   void                       *net_obj,
                   OENET_listen_for_connect   *listen,
                   OENET_cleanup_conn         *orderly_cleanup_conn_fun,
                   OENET_connect              *connect_fun,
                   OENET_listen_for_bytes     *listen_for_bytes_fun,
                   OENET_new_conn             *new_conn_fun);

extern void OeNet_free( T* );

//pkg impl api (do not use)
extern oenet_callback *OeNet_get_read_handler(T);
extern oenet_callback *OeNet_get_con_timeout_cb(T);
extern void *OeNet_get_net_obj(T);
//extern void OeNet_set_net_obj(T _this_, void *net_obj);
extern oe_id OeNet_create_id(T);

//user's config api
extern void OeNet_set_read_handler(T, oenet_callback *);
extern void OeNet_set_con_timeout_cb(T, oenet_callback *);
extern void OeNet_set_idgen(T, oe_id (*) ( void * ), void *);

//user's mgt api
extern void OeNet_stop(T);
extern void OeNet_stats(T);
extern void *OeNet_get_proto( T );
extern void OeNet_set_proto(T, void *);
extern Oec_IdSet OeNet_get_sessions( T );

//user's networking api
extern void *OeNet_connect(T, const char *, int);
extern OeNetConn OeNet_new_conn(T);
//extern void OeNet_cleanup_conn(OeSession);
extern void OeNet_cleanup_conn(void *);
//extern OeSession OeNet_listen_for_bytes( T, void *);
extern void * OeNet_listen_for_bytes( T, void *);
extern void OeNet_listen_for_connect(T, char *, int);

extern void OeNet_set_name(T, oe_scalar);
extern oe_scalar OeNet_get_name(T);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

