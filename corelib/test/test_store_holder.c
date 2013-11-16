#include "config.h"
#undef NDEBUG
#include <assert.h>
#include "mem.h"
#include "DataObject.h"
#include "DataObjectList.h"
#include "OeStoreHolder.h"
#include "Iterator.h"

#include "oec_values.h"

void test_data_object(Arena_T arena) {

    DataObjectList dlist = DataObjectList_new(arena);
    DataObject d1 = DataObjectList_new_data_object(dlist);
    DataObject_add_attr(d1, "one");
    DataObject_add_attr(d1, "secret1");

    char *bytes = "abcd";
    int nbytes = strlen(bytes);
    DataObject_set_nbytes(d1, nbytes);
    DataObject_set_bytes(d1, bytes);
    char *newbytes = DataObject_get_bytes(d1);
    assert(strcmp(bytes, newbytes) == 0);

    OeStoreHolder h = OeStoreHolder_new_with_data(arena, d1);
    char *mybytes;
    int mysize;
    int rc = OeStoreHolder_marshal(h, &mybytes, &mysize, arena);
    printf("OeStoreHolder_marshal sz: %i\n", mysize);

    OeStoreHolder newh = OeStoreHolder_new_from_buffer(arena, mybytes);
    assert(newh);
    DataObject newdata = OeStoreHolder_get_data(newh);
    assert(newdata);
    assert(strcmp(bytes, DataObject_get_bytes(newdata)) == 0);
}

//suite
int main(void) {

    Arena_T arena = Arena_new();

    test_data_object(arena);

    Arena_dispose(&arena);

    return 0;
}

