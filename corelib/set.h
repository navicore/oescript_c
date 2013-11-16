/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

/* $Id: set.h 6 2007-01-22 00:45:22Z drhanson $ */
#ifndef SET_INCLUDED
#define SET_INCLUDED

#define T Set_T
struct set_ts {
	int length;
	unsigned timestamp;
	int (*cmp)(const void *x, const void *y);
	unsigned (*hash)(const void *x);
	int size;
	struct member {
		struct member *link;
		const void *member;
	} **buckets;
};
typedef struct set_ts *T;

extern T    Set_new (int hint,
	int cmp(const void *x, const void *y),
	unsigned hash(const void *x));
extern void Set_free(T *set);
extern int   Set_length(T set);
extern int   Set_member(T set, const void *member);
extern void  Set_put   (T set, const void *member);
extern void *Set_remove(T set, const void *member);
extern void   Set_map    (T set,
	void apply(const void *member, void *cl), void *cl);
extern void **Set_toArray(T set, void *end);
extern T Set_union(T s, T t);
extern T Set_inter(T s, T t);
extern T Set_minus(T s, T t);
extern T Set_diff (T s, T t);
#undef T
#endif

#ifdef __cplusplus
}
#endif 

