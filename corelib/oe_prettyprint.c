/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include <syslog.h>
#include "mem.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "oec_values.h"
#include <time.h>

static char *HEX_CHARS = "0123456789abcdef";

#ifdef OE_SYSLOG
static void printf_(char *line) {
    syslog(LOG_DEBUG, line);
}
#else 
static void printf_(char *line) {
    fprintf(stdout, line);
}
#endif

static char SEP = ' ';

static void printHex(char *bytes, int bytes_len, int offset, int width, char *l) {

    char *line = l;

    int len;
    if (offset + width > (bytes_len)) {
        len = bytes_len - offset;
    } else {
        len = width;
    }
    for (int i = 0; i < len; i++) {
        char ch = HEX_CHARS[bytes[i + offset] >> 4 & 0x0F];
        *line = ch; line++;
        ch = HEX_CHARS[bytes[i + offset] & 0x0F];
        *line = ch; line++;
        if (i < len - 1) {
            *line = SEP; line++;
        }
    }
    if (len < width) {
        int padsize = width - len;
        for (int i = 0; i < padsize; i++) {
            *line = SEP; line++;
            *line = SEP; line++;
            if (i + 1 < padsize) { //if not last ch
                *line = SEP; line++;
            }
        }
        *line = SEP; line++;
    }
    *line = (int) NULL;
}

static bool isWhitespace(char c) {
    if (c == '\n') return true;
    if (c == '\t') return true;
    if (c == '\r') return true;
    if (c == ' ') return true;
    return false;
}

static void printAscii(char *b, int bytes_len, int index, int width, char *l) {

    char *line = l;
    char *bytes = b;

    if (index < bytes_len) {

        width = (index + width) > bytes_len ? bytes_len - index : width;

        int start = index;
        int stop = index + width;
        for (int i = start; i < stop; i++) {

            char c = bytes[i];
            if (isWhitespace(c)) {
                *line = ' '; line++;
            } else if ( ((int) c) >= 32 && ((int) c) <= 126 ) {
                *line = c; line++;
            } else {
                *line = '.'; line++;
            }
        }
    }
    *line = (int) NULL;

    return;
}

int WIDTH = 16;
void oec_print_hex_and_ascii( char *b, int bytes_len ) {

    char *bytes = b;

    assert(bytes_len > 0);
    assert(bytes);

    char hex[WIDTH * 3 + 1];
    char asci[WIDTH * 2];

    for (int index = 0; index < bytes_len; index += WIDTH) {

        int tmpwidth = WIDTH;

        printHex( bytes, bytes_len, index, tmpwidth, (char *) &hex );
        printAscii( bytes, bytes_len, index, tmpwidth, (char *) &asci );

        char *printme = oec_str_concat(NULL, 5, &hex, "  ", ":", &asci, "\n");
        printf_(printme);
        Mem_free(printme, __FILE__, __LINE__);
    }
}

static int BSZ = 50;
int oec_print_timestamp_to_str(Arena_T arena, char **out, oe_time utime) {

    char *buff;
    if (arena == NULL) {
        buff = Mem_calloc(1, BSZ, __FILE__, __LINE__);
    } else {
        buff = Arena_calloc(arena, 1, BSZ, __FILE__, __LINE__);
    }
    time_t ltime;  
    struct tm *Tm;  

    ltime=utime / 1000;
    Tm=localtime(&ltime);  

    struct timeval detail_time;  
    gettimeofday(&detail_time,NULL);  
    snprintf(buff, BSZ, TS_LONG_FORMAT, Tm->tm_hour,  Tm->tm_min,  Tm->tm_sec, (int) detail_time.tv_usec / 1000,
              Tm->tm_mon + 1, Tm->tm_mday, Tm->tm_year + 1900);
    *out = buff;
    return 0;
}

