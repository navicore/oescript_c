/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEJCL_ENCODER_H
#define OEJCL_ENCODER_H

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///                                                         ///
/// turns oejcl into OEK commands                           ///
///   run each command returned from 'next_cmd' until null  ///
///     passing the optional result from the prev cmd so    ///
///     that variables can be resolved                      ///
///   then 'get_result' to send to client                   ///
///                                                         ///
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

#include "arena.h"
#include "OekMsg.h"

#define T OeJclEncoder
typedef struct T *T;

extern T OeJclEncoder_new(Arena_T);

extern char *OeJclEncoder_msg_serialize(T, const OekMsg);

#undef T

#endif

#ifdef __cplusplus
}
#endif 

