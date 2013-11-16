/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OE_ASSERT_H
#define OE_ASSERT_H

#include "config.h"
#include <assert.h>

//ALWAYS and NEVER are for tests that are always true or false
// but unlike assert, should be tested in production and also
// unlike assert NOT blow things up

#ifdef OE_COVERAGE

#define ALWAYS(X)  (1)
#define NEVER(X)   (0)

#else

#ifdef OE_DEBUG
#define ALWAYS(X)  ((X)?1:assert(0),0)
#define NEVER(X)   ((X)?assert(0),1:0)
#else
#define ALWAYS(X)  (X)
#define NEVER(X)   (X)
#endif

#endif

#endif

#ifdef __cplusplus
}
#endif 

