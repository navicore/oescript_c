#ifdef __cplusplus
extern "C" {
#endif 

/* $Id: table.h 6 2007-01-22 00:45:22Z drhanson $ */
//created from drhanson's table.h Nov 2011 by OnExtent. see LICENSE* files for credits.
#ifndef ARENA_TABLE_INCLUDED
#define ARENA_TABLE_INCLUDED
#include "arena.h"

#define T ArenaTable_T
struct arena_table_T {
    Arena_T arena;
	int size;
	int (*cmp)(const void *x, const void *y);
	unsigned (*hash)(const void *key);
	int length;
	unsigned timestamp;
	struct binding {
		struct binding *link;
		const void *key;
		void *value;
	} **buckets;
};
typedef struct arena_table_T *T;

extern T    ArenaTable_new (Arena_T arena, int hint,
	int cmp(const void *x, const void *y),
	unsigned hash(const void *key));
extern int   ArenaTable_length(T table);
extern void *ArenaTable_put   (T table, const void *key,
	void *value);
extern void *ArenaTable_get   (T table, const void *key);
extern void *ArenaTable_remove(T table, const void *key);
extern void   ArenaTable_map    (T table,
	void apply(const void *key, void **value, void *cl),
	void *cl);
extern void **ArenaTable_toArray(T table, void *end);
extern void ArenaTable_free(T *table);
#undef T
#endif

#ifdef __cplusplus
}
#endif 

