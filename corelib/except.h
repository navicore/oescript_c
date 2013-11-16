/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

/* $Id: except.h 6 2007-01-22 00:45:22Z drhanson $ */
#ifndef EXCEPT_INCLUDED
#define EXCEPT_INCLUDED
#include <config.h>
#include <setjmp.h>

#define T Except_T

////////////////////////////////////////////
////////////////////////////////////////////
/// contains OE modifications to support ///
/// thread local exceptions              ///
/// invalid reads all at END_TRY. 7/5/9  ///
////////////////////////////////////////////
////////////////////////////////////////////

typedef struct except_T {
	const char *reason;
} T;

typedef struct Except_Frame Except_Frame;
struct Except_Frame {
	Except_Frame *prev;
	jmp_buf env;
	const char *file;
	int line;
	const T *exception;
};
enum { Except_entered=0, Except_raised,
       Except_handled,   Except_finalized };

#ifndef WIN32
#ifdef OE_USE_THREADS
#include <pthread.h>
#define ThreadData_T pthread_key_t
#define ThreadData_set(key, value) pthread_setspecific((key), (value))
#define ThreadData_get(key) pthread_getspecific((key))
#define ThreadData_create(key) pthread_key_create(&(key), NULL);
extern ThreadData_T Except_stack;
#define pop_except_stack ThreadData_set(Except_stack, ((Except_Frame*)ThreadData_get(Except_stack))->prev)
#else
extern Except_Frame *Except_stack;
#define pop_except_stack Except_stack = Except_stack->prev
#endif
#else
extern Except_Frame *Except_stack;
#define pop_except_stack Except_stack = Except_stack->prev
#endif

extern void Except_init(void);
extern const Except_T Assert_Failed;
void Except_raise(const T *e, const char *file,int line);
#ifdef WIN32
#include <windows.h>

extern int Except_index;
extern void Except_push(Except_Frame *fp);
extern void Except_pop(void);
#endif
#ifdef WIN32
/* $Id: except.h 6 2007-01-22 00:45:22Z drhanson $ */
#define RAISE(e) Except_raise(&(e), __FILE__, __LINE__)
#define RERAISE Except_raise(Except_frame.exception, \
	Except_frame.file, Except_frame.line)
#define OE_RETURN switch (Except_pop(),0) default: return
#define TRY do { \
	volatile int Except_flag; \
	Except_Frame Except_frame; \
	if (Except_index == -1) \
		Except_init(); \
	Except_push(&Except_frame);  \
	Except_flag = setjmp(Except_frame.env); \
	if (Except_flag == Except_entered) {
#define EXCEPT(e) \
		if (Except_flag == Except_entered) Except_pop(); \
	} else if (Except_frame.exception == &(e)) { \
		Except_flag = Except_handled;
#define ELSE \
		if (Except_flag == Except_entered) Except_pop(); \
	} else { \
		Except_flag = Except_handled;
#define FINALLY \
		if (Except_flag == Except_entered) Except_pop(); \
	} { \
		if (Except_flag == Except_entered) \
			Except_flag = Except_finalized;
#define END_TRY \
		if (Except_flag == Except_entered) Except_pop(); \
		} if (Except_flag == Except_raised) RERAISE; \
} while (0)
#else
#define RAISE(e) Except_raise(&(e), __FILE__, __LINE__)
#define RERAISE Except_raise(Except_frame.exception, \
	Except_frame.file, Except_frame.line)
//#define RETURN switch (Except_stack = Except_stack->prev,0) default: return
#ifdef OE_USE_THREADS
#define OE_RETURN switch (pop_except_stack,0) default: return
#else
#define OE_RETURN switch ( Except_stack = Except_stack->prev, 0) default: return
#endif

#ifdef OE_USE_THREADS
#define TRY do { \
    Except_init(); \
	volatile int Except_flag; \
	Except_Frame Except_frame; \
    Except_frame.prev = ThreadData_get(Except_stack); \
    ThreadData_set(Except_stack, &Except_frame); \
	Except_flag = setjmp(Except_frame.env); \
	if (Except_flag == Except_entered) {
#else
#define TRY do { \
	volatile int Except_flag; \
	Except_Frame Except_frame; \
    Except_frame.prev = Except_stack; \
    Except_stack = &Except_frame; \
	Except_flag = setjmp(Except_frame.env); \
	if (Except_flag == Except_entered) {
#endif

#define EXCEPT(e) \
		if (Except_flag == Except_entered) pop_except_stack; \
	} else if (Except_frame.exception == &(e)) { \
		Except_flag = Except_handled;
#define ELSE \
		if (Except_flag == Except_entered) pop_except_stack; \
	} else { \
		Except_flag = Except_handled;
#define FINALLY \
		if (Except_flag == Except_entered) pop_except_stack; \
	} { \
		if (Except_flag == Except_entered) \
			Except_flag = Except_finalized;
#define END_TRY \
		if (Except_flag == Except_entered) pop_except_stack; \
		} if (Except_flag == Except_raised) RERAISE; \
} while (0)

#endif

#undef T
#endif

#ifdef __cplusplus
}
#endif 

