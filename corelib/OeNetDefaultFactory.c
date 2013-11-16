/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "OeLibevent2Factory.h"
#include "OeNetDefaultFactory.h"
#include "Oed_Dispatcher.h"

OeNet OeNetDefaultFactory_create(Oed_Dispatcher dispatcher) {
#if defined(OE_USE_LIBEVENT)
    OeNet net = OeLibevent2Factory_create(dispatcher, NULL, NULL, OE_TCP);
    assert(net);
    return net;
#else
    assert(false);
    return NULL;
#endif
}

OeNet OeNetDefaultSslFactory_create(Oed_Dispatcher dispatcher, const char *cert, const char *pkey) {
#if defined(OE_USE_LIBEVENT) && defined(OE_USE_SSL)
    OeNet net = OeLibevent2Factory_create(dispatcher, cert, pkey, OE_TCP_SSL);
    assert(net);
    return net;
#else
    assert(false);
    return NULL;
#endif
}

