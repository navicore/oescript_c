/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

static char rcsid[] = "$Id: table.c 6 2007-01-22 00:45:22Z drhanson $";
#include <assert.h>
#include "mem.h"
#include "table.h"
#include "arenatable.h"

#define T Table_T
T Table_new(int hint,
	int cmp(const void *x, const void *y),
	unsigned hash(const void *key)) {
	T _this_;
	_this_ = OE_ALLOC(sizeof(*_this_));
	_this_->atable = ArenaTable_new(NULL, hint, cmp, hash);
	return _this_;
}
void *Table_get(T table, const void *key) {
	return ArenaTable_get(table->atable, key);
}
void *Table_put(T table, const void *key, void *value) {
	return ArenaTable_put(table->atable, key, value);
}
int Table_length(T table) {
	return ArenaTable_length(table->atable);
}
void Table_map(T table,
	void apply(const void *key, void **value, void *cl),
	void *cl) {
	ArenaTable_map(table->atable, apply, cl);
}
void *Table_remove(T table, const void *key) {
	return ArenaTable_remove(table->atable, key);
}
void **Table_toArray(T table, void *end) {
	return ArenaTable_toArray(table->atable, end);
}
void Table_free(T *table) {
	T t = *table;
	ArenaTable_free(&t->atable);
	OE_FREE(t);
}

#undef T

