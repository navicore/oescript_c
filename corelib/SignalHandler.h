/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef SignalHandler_H
#define SignalHandler_H

#include <stdio.h>
#include <list.h>

#define T SignalHandler
struct sh_T {

    List_T objects;

    const char *pid_file_name;
};
typedef struct sh_T *T;

typedef void user_sig_callback(void *);

extern T    SignalHandler_new(const char *, const char *, const char *);

extern void SignalHandler_manage(T,
                                 user_sig_callback *, 
                                 user_sig_callback *, 
                                 user_sig_callback *, 
                                 void *);

extern void SignalHandler_free(T*);

extern void SignalHandler_shutdown(T _this_);

extern void SignalHandler_diag(T _this_);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

