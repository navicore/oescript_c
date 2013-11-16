/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEK_DEFAULT_FACTORY_H
#define OEK_DEFAULT_FACTORY_H
#include "OEK.h"
#include "Oed_Dispatcher.h"
#include "Oe_Thread_Dispatcher.h"
#include "OeStore.h"

#define T OEK

extern T (OEK_default_factory_new)(Oed_Dispatcher, Oe_Thread_Dispatcher, OeStore);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

