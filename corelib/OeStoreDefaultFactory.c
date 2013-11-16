/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "OeStore.h"
#ifdef OE_USE_DB
#include "OeBdb.h"
#else
#include "OeSqlite.h"
#endif
#include "OeStoreDefaultFactory.h"

OeStore OeStoreDefaultFactory_create(int persist_level, const char *homedir, bool threaded) {
#ifdef OE_USE_DB
    return OeBdb_new(persist_level, homedir, threaded);
#elif defined(OE_USE_SQLITE3)
    return OeSqlite_new(persist_level, homedir, threaded);
#else
    assert(false);
    return NULL;
#endif
}

