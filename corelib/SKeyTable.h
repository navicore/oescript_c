/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

/* $Id: table.h 6 2007-01-22 00:45:22Z drhanson $ */
#ifndef SKEYTABLE_INCLUDED
#define SKEYTABLE_INCLUDED

#include "table.h"
#include "arenatable.h"
#include "arena.h"

extern Table_T SKeyTable_new (int hint);
extern ArenaTable_T SKeyTable_arena_new(Arena_T, int);

#endif

#ifdef __cplusplus
}
#endif 

