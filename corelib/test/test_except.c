#include "config.h"
#undef NDEBUG
#include <assert.h>

#include "except.h"
#include <mem.h>
#include <stdbool.h>
#include <stdio.h>

const Except_T OeTest_Failed    = { "Test Exception Failed Succeeded" };

void test_finally_1(void) {

    volatile int c = 0;

    TRY
        c++;
    FINALLY
        c++;
    END_TRY;

    assert(c == 2);
}

volatile int tc = 0;
void callme(void) {
    TRY
        if (true) { RAISE(Mem_Failed); }
        tc += 1000;
    FINALLY
        printf("    iran callme finally\n");
        tc++;
    END_TRY;

    return;
}
void outer_callme(void) {
    TRY
        callme();
        tc += 2000;
    EXCEPT(OeTest_Failed)
        printf("  iran outer_callme e1\n");
        tc += 10000;
        RERAISE;
    EXCEPT(Mem_Failed)
        printf("  iran outer_callme e2\n");
        tc++;
        RERAISE;
    FINALLY
        //note, finally seems skipped over by RERAISE
        tc += 400000;
        printf("  iran outer_callme finally\n");
    END_TRY;
}
void outer_outer_callme(void) {
    TRY
        outer_callme();
    EXCEPT(OeTest_Failed)
        printf("iran outer_outer_callme e1\n");
        tc += 100000;
    EXCEPT(Mem_Failed)
        printf("iran outer_outer_callme e2\n");
        tc++;
    FINALLY
        //note, this seeems to be the only FINALLY called because no RERAISE
        printf("iran outer_outer_callme finally\n");
        tc++;
    END_TRY;
}

void test_nested_except(void) {
    tc = 0;
    outer_outer_callme();
    printf("iran %d\n", tc);
    assert(tc == 4); //should incr once in each finally and once in
                        //the inner except handler and the 2000 after the
                        //good outercall
}

volatile int rc = 0;
void returnme(void) {
    TRY
        rc++;
        OE_RETURN;
        rc += 100;
    EXCEPT(Mem_Failed)
        rc += 2000;
    FINALLY
        rc += 30000;
    END_TRY;
    rc += 400000;

    return;
}
void test_return(void) {
    assert(!rc);
    returnme();
    //printf("returnme rc: %d\n", rc);
    //assert(rc == 30001); //just to learn that RETURN doesn't go through FINALLY unfortunately
    assert(rc == 1); //unfortunately it works this way
}

//suite
int main(void) {

    test_finally_1();
    test_nested_except();
    test_return();

    return 0;
}

