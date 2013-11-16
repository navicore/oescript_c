static char rcsid[] = "$Id: arena.c 6 2007-01-22 00:45:22Z drhanson $";
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include <assert.h>
#include "except.h"
#include "arena.h"
#define T Arena_T

////////////////////////////////////////////
////////////////////////////////////////////
/// contains OE modifications to support ///
/// thread local chunks          8/10/9  ///
////////////////////////////////////////////
////////////////////////////////////////////
#include <pthread.h>
#define ThreadData_T pthread_key_t
#define ThreadData_set(key, value) pthread_setspecific((key), (value))
#define ThreadData_get(key) pthread_getspecific((key))
#define ThreadData_create(key) pthread_key_create(&(key), NULL);

ThreadData_T freechunks_td;
static pthread_once_t freechunks_once_control = PTHREAD_ONCE_INIT;
static void init_freechunks(void) {ThreadData_create(freechunks_td); }
#define init_freechunks_once() pthread_once(&(freechunks_once_control), init_freechunks)

ThreadData_T nfree_td;
static pthread_once_t nfree_once_control = PTHREAD_ONCE_INIT;
static void init_nfree(void) {ThreadData_create(nfree_td); }
#define init_nfree_once() pthread_once(&(nfree_once_control), init_nfree)

void set_freechunks(T freechunks) {
	init_freechunks_once();
	ThreadData_set(freechunks_td, freechunks);
}
T get_freechunks(void) {
	init_freechunks_once();
	return ThreadData_get(freechunks_td);
}

void set_nfree(int nfree) {
	init_nfree_once();
	ThreadData_set(nfree_td, (void *) nfree);
}
int get_nfree(void) {
	init_nfree_once();
	return (int) ThreadData_get(nfree_td);
}

const Except_T Arena_NewFailed =
	{ "Arena Creation Failed" };
const Except_T Arena_Failed    =
	{ "Arena Allocation Failed" };
#define THRESHOLD 10

union align {
#ifdef MAXALIGN
	char pad[MAXALIGN];
#else
	int i;
	long l;
	long *lp;
	void *p;
	void (*fp)(void);
	float f;
	double d;
	long double ld;
#endif
};
union header {
	struct arena_T *b;
	union align a;
};
//static T freechunks;
//static int nfree;
T Arena_new(void) {
	T arena = malloc(sizeof (*arena));
	if (arena == NULL)
		RAISE(Arena_NewFailed);
	arena->prev = NULL;
	arena->limit = arena->avail = NULL;
	return arena;
}
void Arena_dispose(T *ap) {
	assert(ap && *ap);
	Arena_free(*ap);
	free(*ap);
	*ap = NULL;
}
void *Arena_alloc(T arena, long nbytes,
	const char *file, int line) {
	assert(arena);
	assert(nbytes > 0);
	nbytes = ((nbytes + sizeof (union align) - 1)/
		(sizeof (union align)))*(sizeof (union align));
	while (nbytes > arena->limit - arena->avail) {
		T ptr;
		char *limit;
		if ((ptr = get_freechunks()) != NULL) {
			//freechunks = freechunks->prev;
			set_freechunks(ptr->prev);
			//nfree--;
			set_nfree(get_nfree() - 1);
			limit = ptr->limit;
		} else {
			long m = sizeof (union header) + nbytes + 10*1024;
			ptr = malloc(m);
			if (ptr == NULL)
				{
					if (file == NULL)
						RAISE(Arena_Failed);
					else
						Except_raise(&Arena_Failed, file, line);
				}
			limit = (char *)ptr + m;
		}
		*ptr = *arena;
		arena->avail = (char *)((union header *)ptr + 1);
		arena->limit = limit;
		arena->prev  = ptr;
	}
	arena->avail += nbytes;
	return arena->avail - nbytes;
}
void *Arena_calloc(T arena, long count, long nbytes,
	const char *file, int line) {
	void *ptr;
	assert(count > 0);
	ptr = Arena_alloc(arena, count*nbytes, file, line);
	memset(ptr, '\0', count*nbytes);
	return ptr;
}
void Arena_free(T arena) {
	assert(arena);
	while (arena->prev) {
		struct arena_T tmp = *arena->prev;
		if (get_nfree() < THRESHOLD) {
			arena->prev->prev = get_freechunks();
			//freechunks = arena->prev;
			set_freechunks(arena->prev);
			//nfree++;
			set_nfree(get_nfree() + 1);
			//freechunks->limit = arena->limit;
			arena->prev->limit = arena->limit;
		} else
			free(arena->prev);
		*arena = tmp;
	}
	assert(arena->limit == NULL);
	assert(arena->avail == NULL);
}

#undef T

