/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEBDB_H
#define OEBDB_H

#include "OeStore.h"
#include <stdbool.h>

///////////////////////////////////
// DB                            //
///////////////////////////////////

#define T OeBdb
typedef struct T *T;

OeStore (OeBdb_new)(int persist_level, const char *homedir, bool threaded);

#undef T

#endif


#ifdef __cplusplus
}
#endif 

