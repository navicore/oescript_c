/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

/* $Id: H:/drh/idioms/book/RCS/list.doc,v 1.11 1997/02/21 19:46:01 drh Exp $ */
/*
  file forked and renamed 9/2008 for onextent name space and inclusion in onextent
  open source software.  see file cii_COPYRIGHT.txt for dhr's license.
*/
#ifndef ALIST_INCLUDED
#define ALIST_INCLUDED
#include "arena.h"
#include "ArrayIterator.h"
#define T Oec_AList

struct alist_T {
	struct alist_T *rest;
	void *first;
};
typedef struct alist_T *T;

extern T      Oec_AList_append (T list, T tail);
extern T      Oec_AList_list   (Arena_T arena, void *x, ...);
extern int    Oec_AList_length (T list);
extern void **Oec_AList_toArray(T list, Arena_T arena, void *end);
extern void  *Oec_AList_get    (T list, int);
extern T      Oec_AList_pop(T list, void **x);

extern ArrayIterator Oec_AList_iterator(T);

#undef T
#endif
#ifdef __cplusplus
}
#endif 

