/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEKMSGTEMPLATEMATCHER_H
#define OEKMSGTEMPLATEMATCHER_H

#include <stdbool.h>
#include "OekMsg.h"

extern bool OekMsgTemplateMatcher_match(OekMsg msg, OekMsg mtemplate);

#endif

#ifdef __cplusplus
}
#endif 

