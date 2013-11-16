/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEPUTILS_H
#define OEPUTILS_H

#include "oec_values.h"
#include "arena.h"

extern oe_scalar oepu_get_extentname_from_spec(Arena_T, const char *);

extern char *oepu_get_hostname_from_spec(Arena_T, const char *);

extern int oepu_get_port_from_spec(const char *);

#endif

#ifdef __cplusplus
}
#endif 

