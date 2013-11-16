/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OELIBEVENT_FACTORY_H
#define OELIBEVENT_FACTORY_H

#include "Oed_Dispatcher.h"
#include "OeNet.h"

enum __net_type {
    DEFAULT, OE_TCP, OE_TCP_SSL
};
typedef enum __net_type NET_TYPE; 

extern OeNet OeLibevent2Factory_create(Oed_Dispatcher, const char*, const char*, NET_TYPE);

#endif

#ifdef __cplusplus
}
#endif 

