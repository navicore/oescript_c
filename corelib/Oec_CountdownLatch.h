/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEC_COUNTDOWN_LATCH_INCLUDED
#define OEC_COUNTDOWN_LATCH_INCLUDED

#include "oec_values.h"

#define T Oec_CountdownLatch

typedef struct T *T;

extern int Oec_CountdownLatch_get_hi_water_mark(T);
extern int Oec_CountdownLatch_increment(T);
extern int Oec_CountdownLatch_decrement(T);
extern int Oec_CountdownLatch_get( T, oe_time );

extern T   Oec_CountdownLatch_new(void);
extern int Oec_CountdownLatch_free( T * );

#undef T
#endif

#ifdef __cplusplus
}
#endif 

