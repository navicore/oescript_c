/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OE_DISPATCHER_COMMON_H
#define OE_DISPATCHER_COMMON_H

typedef void user_callback(void *);
typedef void *user_callback_arg;

extern Arena_T Oed_get_arena(void);

void _process_fun(user_callback *fun, void *funarg);

#endif /* OE_DISPATCHER_COMMON_H */

#ifdef __cplusplus
}
#endif 

