/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEJCLNET_H
#define OEJCLNET_H

#include "OeSession.h"
#include "arena.h"
#include "oec_values.h"
#include "OekMsg.h"

//ifc for protocol module to configure itself with
extern int OeJclNet_read_msg(OeSession client, OekMsg *msgp, Arena_T arena);
extern int OeJclNet_write_msg(OeSession client, OekMsg msg, Arena_T arena);
extern int OeJclNet_peek_cid(OeSession client, oe_id *cid);

#endif

#ifdef __cplusplus
}
#endif 

