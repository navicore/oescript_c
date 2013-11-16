static char rcsid[] = "$Id: table.c 6 2007-01-22 00:45:22Z drhanson $";
//created from drhanson's table.c Nov 2011 by OnExtent. see LICENSE* files for credits.
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include "mem.h"
#include "arena.h"
#include "config.h"
#include "arenatable.h"

#define T ArenaTable_T
static int cmpatom(const void *x, const void *y) {
	return x != y;
}
static unsigned hashatom(const void *key) {
	return (unsigned long)key>>2;
}
T ArenaTable_new(Arena_T arena, int hint,
	int cmp(const void *x, const void *y),
	unsigned hash(const void *key)) {
	T table;
	int i;
	static int primes[] = { 509, 509, 1021, 2053, 4093,
		8191, 16381, 32771, 65521, INT_MAX };
	assert(hint >= 0);
	for (i = 1; primes[i] < hint; i++)
		;
    int tlen = sizeof (*table) + primes[i-1]*sizeof(table->buckets[0]);
    if (arena == NULL) {
        table = OE_ALLOC(tlen);
        table->arena = NULL;
    } else {
        table = Arena_alloc(arena, tlen, __FILE__, __LINE__);
        table->arena = arena;
    }
	table->size = primes[i-1];
	table->cmp  = cmp  ?  cmp : cmpatom;
	table->hash = hash ? hash : hashatom;
	table->buckets = (struct binding **)(table + 1);
	for (i = 0; i < table->size; i++)
		table->buckets[i] = NULL;
	table->length = 0;
	table->timestamp = 0;
	return table;
}
void *ArenaTable_get(T table, const void *key) {
	int i;
	struct binding *p;
	assert(table);
	assert(key);
	i = (*table->hash)(key)%table->size;
	for (p = table->buckets[i]; p; p = p->link)
		if ((*table->cmp)(key, p->key) == 0)
			break;
	return p ? p->value : NULL;
}
void *ArenaTable_put(T table, const void *key, void *value) {
	int i;
	struct binding *p;
	void *prev;
	assert(table);
	assert(key);
	i = (*table->hash)(key)%table->size;
	for (p = table->buckets[i]; p; p = p->link)
		if ((*table->cmp)(key, p->key) == 0)
			break;
	if (p == NULL) {
        if (table->arena == NULL) {
            OE_NEW(p);
        } else {
            p = Arena_alloc(table->arena, sizeof(*p), __FILE__, __LINE__);
        }
		p->key = key;
		p->link = table->buckets[i];
		table->buckets[i] = p;
		table->length++;
		prev = NULL;
	} else
		prev = p->value;
	p->value = value;
	table->timestamp++;
	return prev;
}
int ArenaTable_length(T table) {
	assert(table);
	return table->length;
}
void ArenaTable_map(T table,
	void apply(const void *key, void **value, void *cl),
	void *cl) {
	int i;
	unsigned stamp;
	struct binding *p;
	assert(table);
	assert(apply);
	stamp = table->timestamp;
	for (i = 0; i < table->size; i++)
		for (p = table->buckets[i]; p; p = p->link) {
			apply(p->key, &p->value, cl);
			assert(table->timestamp == stamp);
		}
}
void *ArenaTable_remove(T table, const void *key) {
	int i;
	struct binding **pp;
	assert(table);
	assert(key);
	table->timestamp++;
	i = (*table->hash)(key)%table->size;
	for (pp = &table->buckets[i]; *pp; pp = &(*pp)->link)
		if ((*table->cmp)(key, (*pp)->key) == 0) {
			struct binding *p = *pp;
			void *value = p->value;
			*pp = p->link;
            if (table->arena == NULL) {
                OE_FREE(p);
            }
			table->length--;
			return value;
		}
	return NULL;
}
void **ArenaTable_toArray(T table, void *end) {
	int i, j = 0;
	void **array;
	struct binding *p;
	assert(table);
    int alen =  (2*table->length + 1)*sizeof (*array);
    assert(alen > 0);
    if (table->arena == NULL) {
        array = OE_ALLOC(alen);
    } else {
        array = Arena_alloc(table->arena, alen, __FILE__, __LINE__);
    }
	for (i = 0; i < table->size; i++)
		for (p = table->buckets[i]; p; p = p->link) {
			array[j++] = (void *)p->key;
			array[j++] = p->value;
		}
	array[j] = end;
	return array;
}
void ArenaTable_free(T *table) {
	assert(table && *table);
    T t = *table;
    if (t->arena) {
        return;
    }
	if (t->length > 0) {
		int i;
		struct binding *p, *q;
		for (i = 0; i < t->size; i++)
			for (p = t->buckets[i]; p; p = q) {
				q = p->link;
				OE_FREE(p);
			}
	}
	OE_FREE(t);
}
#undef T

