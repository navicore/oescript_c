#include "config.h"
#undef NDEBUG
#include <assert.h>
#include "mem.h"
#include "DataObject.h"
#include "DataObjectList.h"
#include "Iterator.h"

#include "oec_values.h"

void test_subject_api(Arena_T arena) {
    char *STR1 = "mykey1";
    char *STR2 = "~+";
    char *STR3 = "myvalue1";

    char *newstr = oec_str_concat(arena, 3, STR1, STR2, STR3);
    printf("newstr: %s\n", newstr);
    assert(strlen(newstr) == strlen(STR1) + strlen(STR2) + strlen(STR3));
    assert(strcmp( newstr, "mykey2~+myvalue1") != 0);
    assert(strcmp( newstr, "mykey1~+myvalue2") != 0);
    assert(strcmp( newstr, "mykey1=+myvalue1") != 0);
    assert(strcmp( newstr, "mykey1~+myvalue1") == 0);
    if (!arena) {
        Mem_free(newstr, __FILE__, __LINE__);
    }
}

void test_2(Arena_T arena) {

    char *newstr = oec_itoa(arena, 99);
    printf("newstr: %s\n", newstr);
    assert(strlen(newstr) == 2);
    assert(strcmp( newstr, "99") == 0);
    if (!arena) {
        Mem_free(newstr, __FILE__, __LINE__);
    }
}

void test_data_object(Arena_T arena) {

    DataObjectList dlist = DataObjectList_new(arena);
    DataObject d1 = DataObjectList_new_data_object(dlist);
    DataObject_add_attr(d1, "one");
    DataObject_add_attr(d1, "secret1");
    DataObject d2 = DataObjectList_new_data_object(dlist);
    DataObject_add_attr(d2, "two");
    DataObject_add_attr(d2, "secret2");
    DataObject d3 = DataObjectList_new_data_object(dlist);
    DataObject_add_attr(d3, "three");
    DataObject_add_attr(d3, "secret3");
    assert(DataObjectList_length(dlist) == 3);
    int c = 0;
    Iterator iter = DataObjectList_iterator(dlist, false);
    assert(iter);
    for (; Iterator_hasMore(iter); ) {
        printf("d %i hasMore: %i\n", ++c, Iterator_hasMore(iter));
        DataObject data = Iterator_next(iter);
        assert(data);
        printf("d %i has len: %i\n", c, DataObject_get_nattrs(data));
        Iterator iter2 = DataObject_iterator(data, true);
        assert(iter2);
        for (; Iterator_hasMore(iter2); ) {
            printf(" attr: %s\n", Iterator_next(iter2));
        }
    }
    Iterator_free(&iter);

    char *bytes = "abcd";
    int nbytes = strlen(bytes);
    DataObject_set_nbytes(d1, nbytes);
    DataObject_set_bytes(d1, bytes);
    char *newbytes = DataObject_get_bytes(d1);
    assert(strcmp(bytes, newbytes) == 0);
}

void test_unquote() {
  char *quotedname = "\"seeme with no quotes!\"";
  int len = strlen(quotedname);
  char *cpy = oec_unquote(quotedname, len);
  printf("cpy: %s\n", cpy);
  assert(cpy);
}

void test_digit_counters() {
    int c = oec_count_long_digits(123);
    assert(c == 3);
    c = oec_count_long_digits(12);
    assert(c == 2);
    c = oec_count_long_digits(123456789012345);
    assert(c == 15);
}

//suite
int main(void) {

    Arena_T arena = Arena_new();
    test_subject_api(NULL);
    test_subject_api(arena);
    test_2(arena);
    Arena_dispose(&arena);
    arena = Arena_new();
    test_subject_api(arena);
    test_2(arena);

    test_data_object(arena);
    test_unquote();

    test_digit_counters();

    Arena_dispose(&arena);

    return 0;
}

