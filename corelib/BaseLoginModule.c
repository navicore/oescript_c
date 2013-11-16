/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "BaseLoginModule.h"
#include "OekMsgTemplateMatcher.h"
#include "mem.h"
#include "assert.h"
#include "Subject.h"
#include "oec_values.h"
#include "table.h"
#include "SKeyTable.h"
#include "Oec_AList.h"
#include "stdbool.h"
#include "oec_log.h"
#include <string.h>

#define T BaseLoginModule

typedef struct extinfo extinfo;
struct extinfo {
    oe_scalar extentname;
    ArenaTable_T groups; //a list of grpinfo objects
};

typedef struct grpinfo grpinfo;
struct grpinfo {
    oe_scalar groupname;
    Oec_AList templates;
};

static grpinfo *_get_grpinfo(T, oe_scalar, oe_scalar, bool);

static AUTH _allow(grpinfo *ginfo, OekMsg msg) {
    AUTH ret = DONTCARE;
    if (ginfo == NULL) {
        OE_TLOG(0, "BaseLoginModule _allow ginfo not set\n"); 
        return ret; //just because we don't know this user doesn't mean 
                    //some other module in the auth chain doesn't
    }
    int len = Oec_AList_length(ginfo->templates);
    ArrayIterator iter = Oec_AList_iterator(ginfo->templates);
    while (ArrayIterator_hasMore(iter)) {
        OekMsg mtemplate = ArrayIterator_next(iter);
        if (OekMsgTemplateMatcher_match(msg,mtemplate)) {
            OE_TLOG(0, "BaseLoginModule _allow allow = yes\n");
            ret = ALLOW;
            break;
        }
    }
    ArrayIterator_free(&iter);
    return ret;
}

T BaseLoginModule_new(Arena_T arena) {

    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    _this_->extents = SKeyTable_arena_new(arena, 100);

    return _this_;
}

void BaseLoginModule_assign_privs( T _this_, Subject s, oe_scalar *grouplst) {
    assert(_this_);
    assert(s);
    assert(grouplst);
    oe_scalar username = Subject_get_username(s);
    assert(username);
    oe_scalar extentname = Subject_get_extentname(s);
    assert(extentname);

    if (!Subject_is_authenticated(s)) {
        OE_ERR( NULL, "BaseLoginModule can not assign privs to unauthenticated user: %s\n", 
                username);
        return;
    }

    //add privs for each group in grouplst
    oe_scalar gname;
    for (int i = 0; i < MAX_GROUPS; i++) {
        gname = grouplst[i];
        if (gname == NULL) {
            break;
        }

        OE_DLOG( NULL, "BaseLoginModule _login adding privs for user: %s grp: %s\n", 
                 username, gname); //i18n
        grpinfo *ginfo = _get_grpinfo(_this_, extentname, gname, false);

        if (ginfo != NULL && Subject_is_authenticated(s)) {
            Privilege p = Privilege_new(Subject_get_arena(s), _allow, ginfo);
            Subject_add_privilege(s, p);
        }
    }
}

static grpinfo *_get_grpinfo(T _this_, oe_scalar extentname, oe_scalar groupname, bool alloc) {

    extinfo *einfo = ArenaTable_get(_this_->extents, extentname);
    if (einfo == NULL && alloc) {
        einfo = Arena_calloc(_this_->arena, 1,
                             sizeof (extinfo), __FILE__, __LINE__);
        oe_scalar extname_cpy = oec_ncpy_str(_this_->arena, extentname, strlen(extentname));
        einfo->extentname = extname_cpy;
        einfo->groups = SKeyTable_arena_new(_this_->arena, 100);

        ArenaTable_put(_this_->extents, extname_cpy, einfo);
    }
    if (einfo == NULL) return NULL;
    grpinfo *ginfo = ArenaTable_get(einfo->groups, (void *) groupname);
    if (ginfo == NULL && alloc) {
        ginfo = Arena_calloc(_this_->arena, 1,
                             sizeof (grpinfo), __FILE__, __LINE__);
        oe_scalar grpname_cpy = oec_ncpy_str(_this_->arena, groupname, strlen(groupname));
        ginfo->groupname = grpname_cpy;
        ginfo->templates = Oec_AList_list(_this_->arena, NULL);

        ArenaTable_put(einfo->groups, grpname_cpy, ginfo);
    }
    return ginfo;
}

int BaseLoginModule_add_privilege( T _this_, 
                                   oe_scalar extentname,
                                   oe_scalar groupname,
                                   OekMsg mtemplate) {
    Arena_T arena = _this_->arena;
    oe_scalar grpnamecpy = oec_cpy_str(arena, groupname);
    oe_scalar extnamecpy = oec_cpy_str(arena, extentname);
    grpinfo *ginfo = _get_grpinfo(_this_, extnamecpy, grpnamecpy, true);
    assert(ginfo);

    ginfo->templates = Oec_AList_append(ginfo->templates,
                                        Oec_AList_list(arena, mtemplate, NULL) );
    int len = Oec_AList_length(ginfo->templates);
    OE_DLOG(0, "BaseLoginModule_add_privilege\n\t\text: %s\n\t\tgrp: %s\n\t\t%i\n\t\tnprivs: %i\n", extentname, groupname, OekMsg_get_type(mtemplate), len);
    return 0;
}

#undef T

