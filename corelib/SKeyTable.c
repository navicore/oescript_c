/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "SKeyTable.h"
#include "arenatable.h"
#include "oec_values.h"
#include "arena.h"

static int _stringcmp(const void *x, const void *y) {
    char *xitem = (char *) x;
    char *yitem = (char *) y;
    int xint = oec_hash_string_key(xitem);
    int yint = oec_hash_string_key(yitem);
    int rc;
    if (xint == yint) rc = 0;
    else if (xint < yint) rc = -1;
    else rc = 1;
    return rc;
}
static unsigned _stringhash(const void *x) {
    char *xitem = (char *) x;
    int xint = oec_hash_string_key(xitem);
    return(unsigned) xint;
}

Table_T SKeyTable_new(int hint) {
    return Table_new(hint, _stringcmp, _stringhash);
}

ArenaTable_T SKeyTable_arena_new(Arena_T arena, int hint) {
    return ArenaTable_new(arena, hint, _stringcmp, _stringhash);
}

