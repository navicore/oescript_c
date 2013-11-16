#include "config.h"
#undef NDEBUG
#include <assert.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "oe_common.h"
#include "Oec_AList.h"
#include "Oec_Future.h"
#include "DataObject.h"
#include "DataObjectList.h"

void test_mem_1(void) {

    char *bytes = Mem_alloc( 5, __FILE__, __LINE__);
    OE_FREE(bytes);
    OE_FREE(bytes);
}

void test_getTimestamp(void) {

    oec_print_timestamp(stdout);
}

void test_getTimeMilliseconds(void) {

    oe_time t1 = oec_get_time_milliseconds();
    usleep(20000);
    oe_time t2 = oec_get_time_milliseconds();
    oe_time t3 = oec_get_time_milliseconds();
    OE_ILOG(NULL,"t1: %lld t2: %lld diff: %lld\n", t1, t2, t2 - t1);
    OE_ILOG(NULL,"t2: %lld t3: %lld diff: %lld\n", t2, t3, t3 - t2);
    assert(t2 >= (t1 + 20) && t2 < (t1 + 40)); //usleep is much slower than the timeout val
}

static char *str1 = "RES PUT %lld %ld %lld\n\n";
static char *str2 = "this is a longer test string with some 1234s in it!\n";
void test_pretty_print(void) {
    oec_print_hex_and_ascii( str1, strlen(str1) );
    OE_ILOG(NULL,"\n");
    oec_print_hex_and_ascii( str2, strlen(str2) );
}
void test_pretty_print2(void) {

    char *timestr;
    oe_time now = oec_get_time_milliseconds();
    oec_print_timestamp_to_str(NULL,&timestr,now);
    OE_ILOG(NULL,"time: %s\n", timestr);
    OE_ILOG(NULL,"oe_time: %llu\n", oec_get_time_milliseconds());
    Mem_free(timestr, __FILE__, __LINE__);
}

#define	MY_F1   0x0000001
#define	MY_F2   0x0000002
#define	MY_F3	0x0000004
#define	MY_F4	0x0000008
#define	MY_F5	0x0000010
#define	MY_F6	0x0000020
#define	MY_F7	0x0000040
#define	MY_F8	0x0000080

void test_flags(void) {

    oe_flags my_flag_set;
    memset(&my_flag_set, 0, sizeof(oe_flags));
    assert(!OEC_FLAG_ISSET(my_flag_set, MY_F1));
    OEC_FLAG_SET(my_flag_set, MY_F2);
    assert(!OEC_FLAG_ISSET(my_flag_set, MY_F1));
    assert(OEC_FLAG_ISSET(my_flag_set, MY_F2));
    OEC_FLAG_SET(my_flag_set, MY_F1);
    assert(OEC_FLAG_ISSET(my_flag_set, MY_F1));
    assert(OEC_FLAG_ISSET(my_flag_set, MY_F2));
    assert(!OEC_FLAG_ISSET(my_flag_set, MY_F7));
    OEC_FLAG_SET(my_flag_set, MY_F7);
    assert(OEC_FLAG_ISSET(my_flag_set, MY_F7));
    assert(OEC_FLAG_ISSET(my_flag_set, MY_F1));
    assert(OEC_FLAG_ISSET(my_flag_set, MY_F2));
    OEC_FLAG_CLEAR(my_flag_set, MY_F2);
    assert(!OEC_FLAG_ISSET(my_flag_set, MY_F2));
    assert(OEC_FLAG_ISSET(my_flag_set, MY_F1));
    assert(OEC_FLAG_ISSET(my_flag_set, MY_F7));
}

void test_hash(void) {
    int h1 = oec_hash_string_key("hi");
    printf("h1: %d\n", h1);

    h1 = oec_hash_string_key("hellow world");
    printf("h1: %d\n", h1);
}

//no concurrency testing here.  see the dispatcher pkg tests for more testing
void test_future(void) {

    oe_time SHORT_WAIT = 5;
    oe_time LONG_WAIT = 5000;
    char *msg = "hiya";
    char *res;

    Oec_Future f = Oec_Future_new();
    assert(!Oec_Future_get(f, 0));

    oe_time starttime = oec_get_time_milliseconds();
    res = Oec_Future_get(f, SHORT_WAIT);
    oe_time now = oec_get_time_milliseconds();
    assert(!res);
    assert(now >= (starttime + SHORT_WAIT));

    Oec_Future_set(f, msg);
    res = Oec_Future_get(f, LONG_WAIT);
    now = oec_get_time_milliseconds();
    assert(res);
    assert(res == msg);
    assert(now < (starttime + LONG_WAIT + SHORT_WAIT));
}

void test_list(void) {

    char *myrec = "hi";
    char *myrec2 = "hiya";
    List_T lst = List_list(NULL);
    lst = List_append(lst, List_list(myrec, NULL));
    lst = List_append(lst, List_list(myrec, NULL));
    lst = List_append(lst, List_list(myrec2, NULL));
    //printf("list size: %d\n", List_T_length(lst));
    //printf("myrec2: %s\n", List_T_get(lst, 2));
    /*
    assert(List_get(lst, 2) == myrec2);
    assert(List_get(lst, 1) == myrec);
    assert(List_get(lst, 0) == myrec);
    assert(List_get(lst, 3) == NULL);
    assert(List_get(lst, 5) == NULL);
    */
}

void test_alist(void) {

    Arena_T arena = Arena_new();
    char *myrec = "ho";
    char *myrec2 = "hoya";
    Oec_AList lst = Oec_AList_list(arena, NULL);
    lst = Oec_AList_append(lst, Oec_AList_list(arena, myrec, NULL));
    lst = Oec_AList_append(lst, Oec_AList_list(arena, myrec, NULL));
    lst = Oec_AList_append(lst, Oec_AList_list(arena, myrec2, NULL));
    printf("myrec2: %s\n", (char *) Oec_AList_get(lst, 2));
    assert(Oec_AList_get(lst, 2) == myrec2);
    void *xlst = Oec_AList_get(lst, 2);
    assert(Oec_AList_get(lst, 1) == myrec);
    assert(Oec_AList_get(lst, 0) == myrec);
    assert(Oec_AList_get(lst, 3) == NULL);
    assert(Oec_AList_get(lst, 5) == NULL);
    Arena_dispose(&arena);
}

void test_string_list(void) {

    Arena_T arena = Arena_new();
    DataObjectList batch = DataObjectList_new(arena);
    DataObject strings = DataObjectList_new_data_object(batch);
    DataObject_add_attr(strings, "hiya");

    Iterator iter = DataObjectList_iterator(batch, true);
    DataObject newdo =  Iterator_next(iter);
    char **carray = DataObject_toArray(newdo);
    assert(strcmp("hiya", carray[0]) == 0);
}

static char *needle = "see me";
static char *needle2 = "\n\n";
static char *needle3 = "you can't see me";
static char *haystack = "do you see me?\n\n";
static char *haystack2 = "do you see me?\n";

static void test_memmem(void) {

    void *sub = oe_memmem(haystack, strlen(haystack), needle, strlen(needle));
    assert(sub);
    assert(memcmp(sub, needle, strlen(needle)) == 0);

    sub = oe_memmem(haystack, strlen(haystack), needle2, strlen(needle2));
    assert(sub);
    assert(memcmp(sub, needle2, strlen(needle2)) == 0);

    sub = oe_memmem(haystack, strlen(haystack), needle3, strlen(needle3));
    assert(!sub);

    sub = oe_memmem(haystack2, strlen(haystack2), needle2, strlen(needle2));
    assert(!sub);
}

//suite
int main(void) {

    test_mem_1();

    test_getTimeMilliseconds();

    test_pretty_print();

    test_getTimestamp();

    test_pretty_print2();

    test_flags();

    test_hash();

    test_future();

    test_list();
    test_alist();

    test_string_list();

    test_memmem();

    return 0;
}

