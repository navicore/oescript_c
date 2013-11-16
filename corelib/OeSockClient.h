/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OE_SOCKCLIENT_H
#define OE_SOCKCLIENT_H

#include <sys/types.h>

#define T OeSockClient
typedef struct T *T;

extern T    OeSockClient_new( char *hostname, int port );
extern T    OeSockClient_new_ssl( char *, int);
extern void OeSockClient_free    ( T * );
extern int  OeSockClient_connect ( T );
extern int  OeSockClient_disconnect ( T );
extern int  OeSockClient_send    ( T, char *, size_t );
extern int  OeSockClient_recv    ( T, char *, size_t );

#undef T

#endif

#ifdef __cplusplus
}
#endif 

