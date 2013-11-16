/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OENETDEFAULT_FACTORY_H
#define OENETDEFAULT_FACTORY_H

#include "Oed_Dispatcher.h"

extern OeNet OeNetDefaultFactory_create(Oed_Dispatcher);

extern OeNet OeNetDefaultSslFactory_create(Oed_Dispatcher, const char *, const char *);

#endif

#ifdef __cplusplus
}
#endif 

