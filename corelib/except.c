static char rcsid[] = "$Id: except.c 6 2007-01-22 00:45:22Z drhanson $" "\n$Id: except.c 6 2007-01-22 00:45:22Z drhanson $";
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include <assert.h>
#include "except.h"
#define T Except_T

////////////////////////////////////////////
////////////////////////////////////////////
/// contains OE modifications to support ///
/// thread local exceptions              ///
/// invalid reads all at END_TRY. 7/5/9  ///
////////////////////////////////////////////
////////////////////////////////////////////

#ifndef WIN32
#ifdef OE_USE_THREADS
ThreadData_T Except_stack;
static pthread_once_t once_control = PTHREAD_ONCE_INIT;
static void init_once(void) { ThreadData_create(Except_stack); }
#else
Except_Frame *Except_stack = NULL;
#endif
#endif

void Except_raise(const T *e, const char *file,
	int line) {
#ifdef WIN32
	Except_Frame *p;

	if (Except_index == -1)
		Except_init();
	p = TlsGetValue(Except_index);
#else
#ifdef OE_USE_THREADS
	Except_Frame *p = ThreadData_get(Except_stack);
#else
	Except_Frame *p = Except_stack;
#endif
#endif
	assert(e);
	if (p == NULL) {
		fprintf(stderr, "Uncaught exception");
		if (e->reason)
			fprintf(stderr, " %s", e->reason);
		else
			fprintf(stderr, " at 0x%p", e);
		if (file && line > 0)
			fprintf(stderr, " raised at %s:%d\n", file, line);
		fprintf(stderr, "aborting...\n");
		fflush(stderr);
		abort();
	}
	p->exception = e;
	p->file = file;
	p->line = line;
#ifdef WIN32
	Except_pop();
#else
#ifdef OE_USE_THREADS
    pop_except_stack;
#else
	Except_stack = Except_stack->prev;
#endif
#endif
	longjmp(p->env, Except_raised);
}
#ifdef WIN32
_CRTIMP void __cdecl _assert(void *, void *, unsigned);
#undef assert
#define assert(e) ((e) || (_assert(#e, __FILE__, __LINE__), 0))

int Except_index = -1;
void Except_init(void) {
	BOOL cond;

	Except_index = TlsAlloc();
	assert(Except_index != TLS_OUT_OF_INDEXES);
	cond = TlsSetValue(Except_index, NULL);
	assert(cond == TRUE);
}

void Except_push(Except_Frame *fp) {
	BOOL cond;

	fp->prev = TlsGetValue(Except_index);
	cond = TlsSetValue(Except_index, fp);
	assert(cond == TRUE);
}

void Except_pop(void) {
	BOOL cond;
	Except_Frame *tos = TlsGetValue(Except_index);

	cond = TlsSetValue(Except_index, tos->prev);
	assert(cond == TRUE);
}
#else
void Except_init(void) {
#ifdef OE_USE_THREADS
        pthread_once(&once_control, init_once);
#endif
}
#endif

#undef T

