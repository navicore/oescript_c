/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "mem.h"
#include "oec_values.h"
#include <time.h>
#include <stdarg.h>
#include <assert.h>
#include <stdbool.h>

/* 
#ifdef HAVE_MEMMEM
#include <string.h>
#endif 
*/ 

oe_time oec_get_time_milliseconds(void) {

    struct timeval tv;
    gettimeofday(&tv, NULL);
    return(oe_time) ((oe_time)tv.tv_usec / 1000) + ((oe_time)tv.tv_sec * 1000);
}
//#ifndef HAVE_MEMMEM
void *oe_memmem(const void *haystack, size_t haystacklen,
                const void *needle, size_t needlelen) {
    if (!haystacklen || !needlelen) return NULL;
    char *nchar = (char *) needle;
    void *substr = memchr(haystack, nchar[0], haystacklen);
    if (!substr) return NULL;
    size_t substrlen = haystacklen - (substr - haystack);
    if (substrlen < needlelen) return NULL;
    if (memcmp(substr, needle, needlelen) == 0) {
        return substr;
    } else {
        return oe_memmem(substr + needlelen, substrlen - needlelen, needle, needlelen);
    }
}

char *oec_str_concat(Arena_T arena, int n_args, ...) {
    //dreck how do you pass va_args on?
    //ejs fix this, don't show it to anyone or you'll never work again

    va_list ap;
    int sz =  0;
    char *dest;
    char *args[9];

    va_start(ap, n_args);
    for (int i = 0; i < n_args; i++) {
        args[i] = va_arg(ap, char *);
        sz += strlen(args[i]);
    }
    va_end(ap);
    args[n_args] = NULL;

    assert(sz);

    if (arena) {
        dest = Arena_alloc(arena, sz + 1, __FILE__, __LINE__);
    } else {
        dest = Mem_alloc(sz + 1, __FILE__, __LINE__);
    }
    switch (n_args) {
    case 1:
        sprintf(dest, "%s",
                args[0]);
        break;
    case 2:
        sprintf(dest, "%s%s",
                args[0],
                args[1]);
        break;
    case 3:
        sprintf(dest, "%s%s%s",
                args[0],
                args[1],
                args[2]);
        break;
    case 4:
        sprintf(dest, "%s%s%s%s",
                args[0],
                args[1],
                args[2],
                args[3]);
        break;
    case 5:
        sprintf(dest, "%s%s%s%s%s",
                args[0],
                args[1],
                args[2],
                args[3],
                args[4]);
        break;
    default:
        assert(false);
        sprintf(dest, "%s","err");
    }
    return dest;
}

char *oec_itoa(Arena_T arena, int n) {
    char *dest;
    if (arena) {
        dest = Arena_alloc(arena, 20, __FILE__, __LINE__);
    } else {
        dest = Mem_alloc(20, __FILE__, __LINE__);
    }
    sprintf(dest, "%i", n);
    return dest;
}

/*
#else
void *oe_memmem(const void *haystack, size_t haystacklen,
               const void *needle, size_t needlelen) {
    return memmem(haystack, haystacklen, needle, needlelen);
}
#endif
*/

void oec_print_timestamp(FILE *out) {

    time_t ltime;  
    struct tm *Tm;  

    ltime=time(NULL);  
    Tm=localtime(&ltime);  

    struct timeval detail_time;  
    gettimeofday(&detail_time,NULL);  
    fprintf(out, TS_FORMAT,   
            Tm->tm_hour,  
            Tm->tm_min,  
            (int) Tm->tm_sec,
            (int) detail_time.tv_usec / 1000,
            (int) Tm->tm_mon + 1,
            (int) Tm->tm_mday,
            (int) Tm->tm_year + 1900);
}

static int hash(char *v, int M) {
    int h = 0, a = 127;
    for ( ; *v != '\0'; v++ )
        h = (a*h + *v) % M;
    return h;
}

//hi prob of dupes. use where dupes don't matter much like partitioning sets
int (oec_hash_string_key)(char *key) {
    return hash(key, 99000);
}

char *oec_ncpy_str(Arena_T arena, char *str, int len) {
    assert(str);
    char *cpy = NULL;
    if (arena) {
        cpy = Arena_alloc(arena, len + 1,  __FILE__, __LINE__);
    } else {
        cpy = Mem_alloc(len + 1,  __FILE__, __LINE__);
    }
    strncpy((char *) cpy, (char *) str, len + 1);
    return cpy;
}

oe_scalar oec_cpy_str(Arena_T arena, oe_scalar str) {
    if (!str) return NULL;
    int len = strlen((char *) str);
    return oec_ncpy_str(arena,str,len);
}

char *oec_unquote (const char *s, int len) {
    if (len < 2) return NULL;
    int cpylen = len - 2;
    char *d = (char *)(malloc (cpylen));
    if (d != NULL) {
        strncpy (d,s+1, cpylen);                            
        d[cpylen] = (int) NULL;
    }
    return d;                                 
}

int oec_count_long_digits(long long number)
{
     if (number==0)
          return 0;
     else 
          return 1 + oec_count_long_digits(number/10);
}

