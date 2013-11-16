/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include <stdio.h>
#include "mem.h"
#include <assert.h>

char *oec_read_file(char *filename) {
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    long pos = ftell(f);
    //printf("read pos: %i\n", (int) pos);
    fseek(f, 0, SEEK_SET);

    char *bytes = Mem_alloc((pos + 1) * sizeof(char), __FILE__, __LINE__);
    int n = fread(bytes, (size_t) pos, 1, f);
    bytes[pos] = (int) NULL;
    fclose(f);
    return bytes;
}

