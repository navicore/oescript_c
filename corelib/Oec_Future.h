/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEC_FUTURE_INCLUDED
#define OEC_FUTURE_INCLUDED

#include "oec_values.h"
#include <stdbool.h>
#include <arena.h>

#define T Oec_Future

typedef struct T *T;

/** there are 2 roles, server and client. 
- the server 'alloc's the value and 'set's the value 
- the client 'get's the value AND calls free 
 
don't mess up the roles.  it is really really bad for the server to call 
'free' because the client could later decides to 'get' or 'free'.  
 
if the client 'free's before the server has 'alloc'd or 'set' this is 
handled.  the 'free' has been put off until the 'set' is done.  if you 
never 'set' you will leak.  don't forget to 'set'.  even a NULL is 
considered 'set'.  */
 
//todo: add a callback for get so that the callback is called with the value inline if it is there or
// async when the value is set later.  this will support coroutine clients 

extern bool Oec_Future_is_done( T );
extern void Oec_Future_setid( T, oe_id );
extern void Oec_Future_set( T, void * );
extern void Oec_Future_seterror(T, void *);
extern bool Oec_Future_is_error(T);
extern void *Oec_Future_alloc( T, long );
//timeout is in milliseconds
extern void * Oec_Future_get( T, oe_time );
extern oe_id Oec_Future_getid( T , oe_time );

extern T    Oec_Future_new(void);
extern int Oec_Future_free( T * );

//if you set arena, the delayed safe mem free will also call free on the arena
extern void Oec_Future_set_arena( T, Arena_T );

#undef T
#endif

#ifdef __cplusplus
}
#endif 

