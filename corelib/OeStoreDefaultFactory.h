/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OESTOREDEFAULT_FACTORY_H
#define OESTOREDEFAULT_FACTORY_H

#include "OeStore.h"

extern OeStore OeStoreDefaultFactory_create(int persist_level, const char *homedir, bool threaded);

#endif

#ifdef __cplusplus
}
#endif 

