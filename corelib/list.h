/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

/* $Id: list.h 6 2007-01-22 00:45:22Z drhanson $ */
#ifndef LIST_INCLUDED
#define LIST_INCLUDED
#define T List_T
struct list_T {
	struct list_T *rest;
	void *first;
};
typedef struct list_T *T;

extern T      List_append (T list, T tail);
extern T      List_copy   (T list);
extern T      List_list   (void *x, ...);
extern T      List_pop    (T list, void **x);
extern T      List_push   (T list, void *x);
extern T      List_reverse(T list);
extern int    List_length (T list);
extern void   List_free   (T *list);
extern void   List_map    (T list,
	void apply(void **x, void *cl), void *cl);
extern void **List_toArray(T list, void *end);
#undef T
#endif
#ifdef __cplusplus
}
#endif 

