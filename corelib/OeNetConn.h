/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OENETCONN_H
#define OENETCONN_H

/**
 * API for implementing reading and writing with the async OeNet 
 * API 
 */

#include "config.h"
#include "oec_values.h"

#define T OeNetConn
struct netconn_T;

typedef void   OENETCONN_free_conn_obj(void *);
typedef int    OENETCONN_get_pos      (struct netconn_T*, char *, size_t);
typedef size_t OENETCONN_available    (struct netconn_T*);
typedef size_t OENETCONN_read         (struct netconn_T*, void *, size_t);
typedef int    OENETCONN_write        (struct netconn_T*, char *, size_t);
typedef void  *OENETCONN_peek         (struct netconn_T*, size_t, size_t);
typedef void   OENETCONN_clear_timeout(void *);

struct netconn_T {
    OENETCONN_free_conn_obj *free_conn_obj_fun; 
    OENETCONN_get_pos       *get_pos_fun;
    OENETCONN_available     *available_fun;
    OENETCONN_read          *read_fun;
    OENETCONN_write         *write_fun;
    OENETCONN_peek          *peek_fun;
    OENETCONN_clear_timeout *clear_timeout_fun;
    void *conn_obj;
    void *timeout;
};
typedef struct netconn_T *T;

extern T OeNetConn_new(OENETCONN_free_conn_obj *free_conn_obj_fun,
                       OENETCONN_get_pos       *get_pos_fun,
                       OENETCONN_available     *available_fun,
                       OENETCONN_read          *read_fun,
                       OENETCONN_write         *write_fun,
                       OENETCONN_peek          *peek_fun,
                       OENETCONN_clear_timeout *clear_timeout_fun);
extern void OeNetConn_free( T* );

//impl package internal apis (do not use)
//extern void oenetconn_set_fd( T, int );
//extern int  oenetconn_get_fd( T );
extern void oenetconn_set_conn_obj( T, void * );
extern void *oenetconn_get_conn_obj( T );
extern void  oenetconn_set_timeout(T, void *);
extern void *oenetconn_get_timeout(T);
extern void OeNetConn_free_conn_obj(T);

//the user's network bytes api
extern void  *OeNetConn_peek     (T, size_t, size_t         );
extern int    OeNetConn_get_pos  (T, char *, size_t         );
extern size_t OeNetConn_read     (T, void *, size_t         );
extern int    OeNetConn_write    (T, char *, size_t         );
extern size_t OeNetConn_available(T                         );
extern void   OeNetConn_clear_timeout(T);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

