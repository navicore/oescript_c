#ifdef __cplusplus
extern "C" {
#endif 

/*
 *
 * Licensed Materials - Property of OnExtent, LLC
 * Copyright (c) OnExtent, LLC 2007, 2008, 2009. All rights reserved.
 *
 */
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "oe_common.h"

#define T sn_handler
typedef struct T *T;

void sn_handler_reset(T handler);

void sn_handler_say(T handler, void *name);

bool sn_handler_ran(T handler);

int sn_handler_count(T handler);

oe_time sn_handler_timestamp(T handler);

T sn_handler_new(void);

void sn_handler_free(T handler);

#undef T

#ifdef __cplusplus
}
#endif 

