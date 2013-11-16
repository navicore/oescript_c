/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OE_SUBJECT_H
#define OE_SUBJECT_H

#include "arena.h"
#include "Privilege.h"
#include "ArrayIterator.h"
#include "oec_values.h"

#define T Subject
struct subject_T {

    Arena_T             arena;
    oe_scalar           username;
    oe_scalar           extentname;
    oe_scalar           pwd;
    Oec_AList           privileges;
    Oec_AList           news;
    oe_flags            status;
};

typedef struct subject_T *T;

#define	SUBJECT_AUTHENTICATED   0x0000001
#define	SUBJECT_HAS_NEWS        0x0000002
//#define	SUBJECT_HAS_ERRORS      0x0000004

extern T Subject_new(Arena_T);

extern Arena_T Subject_get_arena(T);

extern void Subject_set_username( T, oe_scalar );
extern oe_scalar Subject_get_username( T );

extern void Subject_set_extentname( T, oe_scalar );
extern oe_scalar Subject_get_extentname( T );

extern void Subject_set_pwd( T, oe_scalar );
extern oe_scalar Subject_get_pwd( T );

extern ArrayIterator Subject_get_privileges(T);
extern void Subject_add_privilege(T, Privilege);

extern bool Subject_status_is_set( T, oe_flags );
extern void Subject_status_clear( T, oe_flags );
extern void Subject_status_set( T, oe_flags );

//
//LoginModule may want to add msg like "password expires in 5 days",
// or "your privileges are 'read only' due to system maint".
//
// 'nothing' is usually the best msg but the motd for 'system going down
// tonight between 3am and 5am PST" might be a good use of 'news'
//
extern ArrayIterator Subject_get_news( T );
extern void Subject_add_news( T , char * );

extern bool Subject_has_news( T );

extern bool Subject_is_authenticated( T );

#undef T

#endif

#ifdef __cplusplus
}
#endif 

