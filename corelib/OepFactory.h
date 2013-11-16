/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEPFACTORY_H
#define OEPFACTORY_H

#include "OepServer.h"
#include "OepClient.h"
#include "Oed_Dispatcher.h"
#include "OeStore.h"
#include "OEK.h"
#include "OeNet.h"

extern OepServer OepFactory_new_server(Oed_Dispatcher, char *, OeNet, OeStore);
extern OepClient OepFactory_new_client(char *,
                                       Oed_Dispatcher, OeNet,
                                       oe_scalar, oe_scalar);

#endif

#ifdef __cplusplus
}
#endif 

