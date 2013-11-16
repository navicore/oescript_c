/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEC_VALUES_H
#define OEC_VALUES_H

#include "arena.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

#define MAX_THREADS 32
#define MAX_LISTENERS 10

typedef long long oe_time; //signed to support forever -1
typedef oe_time     oe_dur;
typedef unsigned long long oe_id;
typedef unsigned long long oe_flags;

//typedef unsigned char *oe_scalar;
typedef char *oe_scalar;
//typedef oe_scalar *oe_chars;

#define	OEC_FLAG_CLEAR(flags, f)    (flags) &= ~(f)
#define	OEC_FLAG_ISSET(flags, f)    ((flags) & (f))
#define	OEC_FLAG_SET(flags, f)      (flags) |= (f)

#define TS_LONG_FORMAT "%02d:%02d:%02d %02d %02d/%02d/%04d"
#define TS_FORMAT      "%02d:%02d:%02d %02d %02d/%02d/%04d\n"

extern oe_time oec_get_time_milliseconds(void);

extern void oec_print_timestamp(FILE *);

extern int oec_print_timestamp_to_str(Arena_T arena, char **, oe_time);

extern void oec_print_hex_and_ascii(char *, int);

#define OEC_MINUTE     60000
#define OEC_HOUR       3600000
#define OEC_DAY        86400000
#define OEC_WEEK       604800000
#define OEC_MONTH      2678400000
#define OEC_YEAR       31536000000
#define OEC_TENYR      315360000000
#define OEC_FOREVER    -1

//hi prob of dupes. use where dupes don't matter much like partitioning sets
extern int (oec_hash_string_key)(char *);

extern void *oe_memmem(const void *, size_t, const void *, size_t);

extern char *oec_str_concat(Arena_T, int, ...);

extern char *oec_itoa(Arena_T, int);

extern char *oec_ncpy_str(Arena_T arena, char *str, int len);
extern oe_scalar oec_cpy_str(Arena_T, oe_scalar);

extern char *oec_unquote (const char *s, int);

extern int oec_count_long_digits(long long number);

#endif

#ifdef __cplusplus
}
#endif 

