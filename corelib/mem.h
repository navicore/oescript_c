/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

/* $Id: mem.h 6 2007-01-22 00:45:22Z drhanson $ */
#ifndef MEM_INCLUDED
#define MEM_INCLUDED
#include "except.h"
extern const Except_T Mem_Failed;
extern void *Mem_alloc (long nbytes,
	const char *file, int line);
extern void *Mem_calloc(long count, long nbytes,
	const char *file, int line);
extern void Mem_free(void *ptr,
	const char *file, int line);
extern void *Mem_resize(void *ptr, long nbytes,
	const char *file, int line);
#define OE_ALLOC(nbytes) \
	Mem_alloc((nbytes), __FILE__, __LINE__)
#define OE_CALLOC(count, nbytes) \
	Mem_calloc((count), (nbytes), __FILE__, __LINE__)
#define OE_NEW(p) ((p) = OE_ALLOC((long)sizeof *(p)))
#define OE_NEW0(p) ((p) = OE_CALLOC(1, (long)sizeof *(p)))
#define OE_FREE(ptr) ((void)(Mem_free((ptr), \
	__FILE__, __LINE__), (ptr) = 0))
#define OE_RESIZE(ptr, nbytes) 	((ptr) = Mem_resize((ptr), \
	(nbytes), __FILE__, __LINE__))
#endif
#ifdef __cplusplus
}
#endif 

