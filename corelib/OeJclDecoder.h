/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEJCL_DECODER_H
#define OEJCL_DECODER_H

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

#define T OeJclDecoder
typedef struct T *T;

extern T OeJclDecoder_new(Arena_T, char *);

extern OekMsg OeJclDecoder_next_cmd(T, OekMsg);

//extern OekMsg OeJclDecoder_get_result(T); //ejs todo

#undef T

#endif

#ifdef __cplusplus
}
#endif 

