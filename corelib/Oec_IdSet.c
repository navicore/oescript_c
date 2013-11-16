/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "Oec_IdSet.h"
#include "set.h"
#include "mem.h"
#include <stdbool.h>

#define T Oec_IdSet

typedef struct item_ {
    oe_id mid;
} ID_ITEM;

static int      itemcmp     (const void *, const void *);
static unsigned itemhash    (const void *);

T Oec_IdSet_new (int hint) {
    T obj;
    obj = Mem_calloc(1, sizeof *obj, __FILE__, __LINE__);
    obj->members = Set_new( 100, itemcmp, itemhash );
    return obj;
}

void Oec_IdSet_free(T *_this_p) {
    assert(_this_p && *_this_p);
    T _this_ = *_this_p;
    //Set_T s = _this_->members;

    void **array = Set_toArray(_this_->members, NULL);
    for ( int i = 0; array[i]; i++ ) {
        ID_ITEM *titem = (ID_ITEM *) array[i];
        if (titem) {
            Mem_free( titem, __FILE__, __LINE__ );
        }
    }
    Mem_free(array, __FILE__, __LINE__);
    Set_free(&_this_->members);
    Mem_free(_this_, __FILE__, __LINE__);
}

int Oec_IdSet_member(T _this_, oe_id mid) {
    ID_ITEM item;
    item.mid = mid;
    int ret = Set_member(_this_->members, &item);
    return ret;
}

void Oec_IdSet_put(T _this_, oe_id mid) {
    ID_ITEM *item;
    item = Mem_alloc(sizeof *item, __FILE__, __LINE__);
    item->mid = mid;
    Set_put(_this_->members, item);
}

oe_id Oec_IdSet_remove(T _this_, oe_id mid) {
    ID_ITEM item;
    item.mid = mid;
    ID_ITEM *removeme = (ID_ITEM *) Set_remove(_this_->members, &item);
    oe_id retid;
    if (removeme != NULL) {
        retid = mid;
    } else {
        retid = (oe_id) 0;
    }
    Mem_free(removeme, __FILE__, __LINE__);
    return retid;
}

//todo: ejs get session count
//todo: ejs get sessions iterator for reporting

static int itemcmp(const void *x, const void *y) {
    ID_ITEM *xitem = (ID_ITEM *) x;
    ID_ITEM *yitem = (ID_ITEM *) y;
    oe_id xint = (oe_id) xitem->mid;
    oe_id yint = (oe_id) yitem->mid;
    if (xint == yint) return 0;
    if (xint < yint) return -1;
    return 1;
}
static unsigned itemhash(const void *x) {
    ID_ITEM *xitem = (ID_ITEM *) x;
    oe_id xint = (oe_id) xitem->mid;
    return(unsigned) xint;
}

#undef T

