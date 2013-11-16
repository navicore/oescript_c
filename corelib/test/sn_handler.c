#include <sn_handler.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "mem.h"
#include "oec_values.h"

//todo: add syncronize to be actually meaningful in tests

#define T sn_handler
struct T {
    bool    ran;
    int     count;
    oe_time timestamp;
};

void sn_handler_say(T handler, void *name) {
    //oe_log(NULL,"my name is %s\n", name);
    handler->ran = true;
    handler->count++;
    handler->timestamp = oec_get_time_milliseconds();
}

void sn_handler_reset(T handler) {
    handler->ran = false;
    handler->count = 0;
    handler->timestamp = 0;
}

bool sn_handler_ran(T handler) {
    return handler->ran;
}

int sn_handler_count(T handler) {
    return handler->count;
}

oe_time sn_handler_timestamp(T handler) {
    return handler->timestamp;
}

T sn_handler_new(void) {
    T handler;
    handler = Mem_calloc(1, sizeof *handler, __FILE__, __LINE__);
    return handler;
}

void sn_handler_free(T handler) {
    Mem_free(handler, __FILE__, __LINE__);
}

#undef T

