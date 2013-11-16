/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef JSONK_H
#define JSONK_H

#include "OEK.h"

struct jsonkernel_T;

#define T JsonKernel

typedef void json_callback(char *, void *);

struct jsonkernel_T {
    OEK kernel;
};

typedef struct jsonkernel_T *T;

extern T JsonKernel_new(OEK kernel);

extern void JsonKernel_free( T* );

extern int JsonKernel_exec(T, char *, json_callback, void *);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

