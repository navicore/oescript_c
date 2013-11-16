/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include "oec_log.h"
#include "oec_values.h"
#include <assert.h>
#include "arena.h"
#include "mem.h"
#include "oep_resources.h"
#include "oep_utils.h"

static char *_strip_proto_from_spec(const char *spec) {

    char *end = (char *) memchr(spec, ':', strlen(spec));
    if (!end) {
        OE_ERR(NULL, OEP_BAD_SPEC, spec);
        return NULL;
    }
    char *hostspec = end + 3;
    return hostspec;
}

oe_scalar oepu_get_extentname_from_spec(Arena_T arena, const char *spec) {

    char *hostspec = _strip_proto_from_spec(spec);
    if (!hostspec) return NULL;

    char *end = (char *) memchr(hostspec, '/', strlen(hostspec));
    if (!end) {
        return NULL;
    }

    int len = end - hostspec;
    char *extentname;
    if (arena) {
        extentname = Arena_calloc(arena, 1, len + 1, __FILE__, __LINE__ );
    } else {
        extentname = Mem_calloc(1, len + 1, __FILE__, __LINE__ );
    }
    strncpy(extentname, end + 1, len);

    return(oe_scalar) extentname;
}

char *oepu_get_hostname_from_spec(Arena_T arena, const char *spec) {

    char *hostspec = _strip_proto_from_spec(spec);
    if (!hostspec) return NULL;

    char *end = (char *) memchr(hostspec, ':', strlen(hostspec));
    if (!end) {
        return NULL;
    }

    int len = end - hostspec;
    char *hostname;
    if (arena) {
        hostname = Arena_calloc(arena, 1, len + 1, __FILE__, __LINE__ );
    } else {
        hostname = Mem_calloc(1, len + 1, __FILE__, __LINE__ );
    }
    strncpy(hostname, hostspec, len);

    return hostname;
}

int oepu_get_port_from_spec(const char *spec) {

    char *hostspec = _strip_proto_from_spec(spec);
    if (!hostspec) return -1;

    char *end = (char *) memchr(hostspec, ':', strlen(hostspec));
    if (!end) {
        return -1;
    }

    int port = atoi(end + 1);

    if (port <= 0) return -1;
    return port;

    return 0;
}

#undef T

