/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

static char rcsid[] = "$Id: H:/drh/idioms/book/RCS/list.doc,v 1.11 1997/02/21 19:46:01 drh Exp $";
/*
  file forked and renamed 9/2008 for onextent name space and inclusion in onextent
  open source software.  see file dhr's cii_COPYRIGHT.txt for license.
*/
#include "config.h"
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include "arena.h"
#include "Oec_AList.h"
#include "mem.h"
#define T Oec_AList
T Oec_AList_list(Arena_T arena, void *x, ...) {
	va_list ap;
	T list, *p = &list;
	va_start(ap, x);
	for ( ; x; x = va_arg(ap, void *)) {
        *p = Arena_alloc(arena, (long) sizeof *p, __FILE__, __LINE__);
		(*p)->first = x;
		p = &(*p)->rest;
	}
	*p = NULL;
	va_end(ap);
	return list;
}
T Oec_AList_append(T list, T tail) {
	T *p = &list;
	while (*p)
		p = &(*p)->rest;
	*p = tail;
	return list;
}
int Oec_AList_length(T list) {
	int n;
	for (n = 0; list; list = list->rest)
		n++;
	return n;
}
void **Oec_AList_toArray(T list, Arena_T arena, void *end) {
	int i, n = Oec_AList_length(list);
	void **array;
    if (arena) {
        array = Arena_alloc(arena, (n + 1)*sizeof (*array), __FILE__, __LINE__);
    } else {
        array = OE_ALLOC((n + 1)*sizeof (*array));
    }
	for (i = 0; i < n; i++) {
		array[i] = list->first;
		list = list->rest;
	}
	array[i] = end;
	return array;
}

void *Oec_AList_get(T list, int pos) {
    void *item;
    if (Oec_AList_length(list) <= pos) return NULL;
    void **array = Oec_AList_toArray(list, NULL, NULL);
    item = array[pos];
    OE_FREE(array);
    return item;    
}

T Oec_AList_pop(T list, void **x) {
	if (list) {
		T head = list->rest;
		if (x)
			*x = list->first;
		//FREE(list);
		return head;
	} else
		return list;
}

////////////////
/// Iterator ///
////////////////

ArrayIterator Oec_AList_iterator(T _this_) {
    return ArrayIterator_new( Oec_AList_toArray( _this_, NULL, NULL ) );
}

#undef T

