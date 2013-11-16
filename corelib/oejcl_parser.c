/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "oejcl_parser.h"
#include "assert.h"
#include "oec_values.h"
#include "oejcl_parser.tab.h"

oejcl_ast *new_oejcl_ast(Arena_T arena, int nodetype, oejcl_ast* l, oejcl_ast* r) {
    assert(arena);
    oejcl_ast *a = Arena_alloc(arena, sizeof(oejcl_ast), __FILE__, __LINE__);
    a->nodetype = nodetype;
    a->l = l;
    a->r = r;
    return a;
}
oejcl_ast *new_oejcl_num_ast(Arena_T arena, int nodetype, double d) {
    oejcl_ast *n = new_oejcl_num(arena, d);
    return new_oejcl_ast(arena, nodetype, n, NULL);
}
oejcl_ast *new_oejcl_num(Arena_T arena, double d) {
    assert(arena);
    numval *a = Arena_alloc(arena, sizeof(numval), __FILE__, __LINE__);
    a->nodetype = NUMBER;
    a->number = d;
    return (oejcl_ast *) a;
}
oejcl_ast *new_oejcl_list_ast(Arena_T arena, char *entry, oejcl_ast* tail) {
    return new_oejcl_ast(arena, LIST, entry, tail);
}
oejcl_ast *new_oejcl_map_ast(Arena_T arena, char *key, char *val, oejcl_ast* tail) {
    oejcl_ast *vast = new_oejcl_ast(arena, MAP, val, tail);
    return new_oejcl_ast(arena, MAP, key, val);
}
oejcl_ast *new_oejcl_str_ast(Arena_T arena, int nodetype, char *s) {
    oejcl_ast *sa = new_oejcl_str(arena, s);
    return new_oejcl_ast(arena, nodetype, sa, NULL);
}


static oejcl_ast *_newstr(Arena_T arena, char *s, int nodetype) {
    assert(arena);
    strval *a = Arena_alloc(arena, sizeof(strval), __FILE__, __LINE__);
    a->nodetype = nodetype;
    a->str = oec_cpy_str(arena, s);
    return (oejcl_ast *) a;
}


oejcl_ast *new_oejcl_str(Arena_T arena, char *s) {
    return _newstr(arena,s,STRING);
}

