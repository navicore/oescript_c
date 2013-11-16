#include "config.h"
#undef NDEBUG
#include <assert.h>

#include "OekMsg.h"
#include "Iterator.h"
#include <stdbool.h>
#include "arena.h"

Arena_T arena;

char *FLD1 = "fld1";
char *FLD2 = "fld222222";
char *FLD3 = "fld333333333";
char *FLD4 = "fld44444444444444";
char *BYTES = "xxxxxxxxxxxx xxxxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

void test_new(void) {
    OekMsg m = OekMsg_new(OEK_ERR,arena);
    OekMsg_set_cid(m, 123);
    m = OekMsg_new(CON_CMD,arena);
    OekMsg_set_cid(m, 123);
    m = OekMsg_new(CON_RES,arena);
    OekMsg_set_cid(m, 123);
    m = OekMsg_new(STA_CMD,arena);
    OekMsg_set_cid(m, 123);
    m = OekMsg_new(STA_RES,arena);
    OekMsg_set_cid(m, 123);
    m = OekMsg_new(UPL_CMD,arena);
    OekMsg_set_cid(m, 123);
    m = OekMsg_new(UPL_RES,arena);
    OekMsg_set_cid(m, 123);
    m = OekMsg_new(UPT_CMD,arena);
    OekMsg_set_cid(m, 123);
    m = OekMsg_new(UPT_RES,arena);
    OekMsg_set_cid(m, 123);
    m = OekMsg_new(PUT_CMD,arena);
    OekMsg_set_cid(m, 123);
    m = OekMsg_new(PUT_RES,arena);
    OekMsg_set_cid(m, 123);
    m = OekMsg_new(GET_CMD,arena);
    OekMsg_set_cid(m, 123);
    m = OekMsg_new(GET_RES,arena);
    OekMsg_set_cid(m, 123);
}

void test_err(void) {
    OekMsg m = OekMsg_new(OEK_ERR,arena);
    OekMsg_set_cid(m, 123);
    OekMsg_set_tid(m, 456);
    OekErr err = OekMsg_get_err(m);
    assert(err);
    char *e = "hi";
    OekErr_set_message(err, e);
    OekErr_set_type(err, 99);
    assert(OekErr_get_type(err) == 99);
    assert(OekErr_get_message(err) == e);
    assert(OekMsg_get_cid(m) == 123);
    assert(OekMsg_get_tid(m) == 456);
    assert(OekMsg_get_type(m) == OEK_ERR);
}
void test_con(void) {
    OekMsg cm = OekMsg_new(CON_CMD,arena);
    ConCmd cmd = OekMsg_get_con_cmd(cm);
    assert(cmd);
    char *user = "me";
    ConCmd_set_username(cmd, user);
    assert(ConCmd_get_username(cmd) == user);

    OekMsg rm = OekMsg_new(CON_RES,arena);
    ConRes res = OekMsg_get_con_res(rm);
    assert(res);
}
void test_sta(void) {
    OekMsg cm = OekMsg_new(STA_CMD,arena);
    StaCmd cmd = OekMsg_get_sta_cmd(cm);
    assert(cmd);
    StaCmd_set_dur(cmd, 55);
    assert(StaCmd_get_dur(cmd) != 56);
    assert(StaCmd_get_dur(cmd) == 55);

    OekMsg rm = OekMsg_new(STA_RES,arena);
    StaRes res = OekMsg_get_sta_res(rm);
    assert(res);
}
void test_upt(void) {
    OekMsg cm = OekMsg_new(UPT_CMD,arena);
    UptCmd cmd = OekMsg_get_upt_cmd(cm);
    assert(cmd);

    OekMsg rm = OekMsg_new(UPT_RES,arena);
    UptRes res = OekMsg_get_upt_res(rm);
    assert(res);
}
void test_upl(void) {
    OekMsg cm = OekMsg_new(UPL_CMD,arena);
    UplCmd cmd = OekMsg_get_upl_cmd(cm);
    assert(cmd);

    OekMsg rm = OekMsg_new(UPL_RES,arena);
    UplRes res = OekMsg_get_upl_res(rm);
    assert(res);
}
void test_put(void) {
    assert(arena);
    OekMsg cm = OekMsg_new(PUT_CMD,arena);
    PutCmd cmd = OekMsg_get_put_cmd(cm);
    assert(cmd);
    DataObject data = PutCmd_get_data(cmd);
    DataObject_add_attr(data, FLD1);
    DataObject_add_attr(data, FLD2);
    DataObject_add_attr(data, FLD3);
    DataObject_add_attr(data, FLD4);
    DataObject_set_nbytes(data, strlen(BYTES) + 1);
    DataObject_set_bytes(data, BYTES);
    assert(DataObject_get_nattrs(data) == 4);
    Iterator iter = DataObject_iterator(data, true);
    assert(Iterator_next(iter) == FLD1);
    assert(Iterator_next(iter) == FLD2);
    assert(Iterator_next(iter) == FLD3);
    assert(Iterator_next(iter) == FLD4);
    assert(DataObject_get_nbytes(data) == strlen(BYTES) + 1);
    assert(DataObject_get_bytes(data) == BYTES);

    OekMsg rm = OekMsg_new(PUT_RES,arena);
    PutRes res = OekMsg_get_put_res(rm);
    assert(res);
}
void test_get(void) {
    OekMsg cm = OekMsg_new(GET_CMD,arena);
    GetCmd cmd = OekMsg_get_get_cmd(cm);
    assert(cmd);

    OekMsg rm = OekMsg_new(GET_RES,arena);
    GetRes res = OekMsg_get_get_res(rm);
    assert(res);
    DataObject data = GetRes_get_data(res);
    DataObject_add_attr(data, FLD1);
    DataObject_add_attr(data, FLD2);
    DataObject_add_attr(data, FLD3);
    DataObject_add_attr(data, FLD4);
    DataObject_set_nbytes(data, strlen(BYTES) + 1);
    DataObject_set_bytes(data, BYTES);
    assert(DataObject_get_nattrs(data) == 4);
    Iterator iter = DataObject_iterator(data, true);
    assert(Iterator_next(iter) == FLD1);
    assert(Iterator_next(iter) == FLD2);
    assert(Iterator_next(iter) == FLD3);
    assert(Iterator_next(iter) == FLD4);
    assert(DataObject_get_nbytes(data) == strlen(BYTES) + 1);
    assert(DataObject_get_bytes(data) == BYTES);
}
void test_flags(void) {
    OekMsg cm = OekMsg_new(GET_CMD,arena);
    GetCmd cmd = OekMsg_get_get_cmd(cm);
    assert(cmd);
    DataObject values = GetCmd_new_data_object(cmd);
    DataObject values2 = GetCmd_new_data_object(cmd);
    assert(!GetCmd_get_join(cmd));
    GetCmd_set_join(cmd);
    assert(GetCmd_get_join(cmd));
}

//suite
int main(void) {

    arena = Arena_new();

    test_flags();
    test_err();
    test_con();
    test_sta();
    test_upt();
    test_upl();
    test_put();
    test_get();
    
    int SZ = 100000;
    oe_time starttime = oec_get_time_milliseconds();

    for (int i = 0; i < SZ; i++) {
        test_new();
    }
    oe_time now = oec_get_time_milliseconds();
    oe_time dur = now - starttime;
    printf("msg create dur: %lld milliseconds for %d msgs\n", dur, SZ);
    Arena_free(arena);
    Arena_dispose(&arena);
    return 0;
}

