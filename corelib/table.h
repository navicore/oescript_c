/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

/* $Id: table.h 6 2007-01-22 00:45:22Z drhanson $ */
#ifndef TABLE_INCLUDED
#define TABLE_INCLUDED
#include "arenatable.h"

#define T Table_T
struct table_T {
	ArenaTable_T atable;
};
typedef struct table_T *T;

extern T    Table_new (int hint,
	int cmp(const void *x, const void *y),
	unsigned hash(const void *key));
extern void Table_free(T *table);
extern int   Table_length(T table);
extern void *Table_put   (T table, const void *key,
	void *value);
extern void *Table_get   (T table, const void *key);
extern void *Table_remove(T table, const void *key);
extern void   Table_map    (T table,
	void apply(const void *key, void **value, void *cl),
	void *cl);
extern void **Table_toArray(T table, void *end);
#undef T
#endif

#ifdef __cplusplus
}
#endif 

