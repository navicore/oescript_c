#include "oejcl_parser.h"
#include "assert.h"
#include "oejcl_parser.tab.h"
#include "OeJclDecoder.h"

int test_batch_1(void) {

    printf("----------- batch -----------\n");

    Arena_T arena = Arena_new();

    char *input = "read(tid=38 cid=2123 timeout=22 [\"one\" \"two\"]);" \
    "take(tid=138 cid=2124 timeout=122 [\"mykey1\": \"myval1\" \"mykey2\" :\"myval2\"]);" \
    "connect(cid=109 extentname=\"myspace\" username=\"me\" password=\"my secret\");" \
    "dis();" \
    "write(tid=77 cid=888 dur=999 [\"mykey11\": \"myval11\" \"mykey22\" :\"myval22\"]);" \
    "txn(cid=8888 timeout=9999);" \
    "commit(cid=111 tid=123 enlisted=1);" \
    "cancel(lid=666);" \
    "commit(cid=1111 tid=1234 enlisted=200);";

    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 2123);
    void *cmd = OekMsg_get_get_cmd(msg);
    assert(cmd);
    assert(!GetCmd_remove(cmd));

    DataObjectList datalist = GetCmd_get_data_objects(cmd);
    assert(DataObjectList_length(datalist) == 1);
    Iterator iter = DataObjectList_iterator(datalist, true);
    DataObject data = Iterator_next(iter);
    int i = 0;
    for ( Iterator di = DataObject_iterator(data, true); Iterator_hasMore(di); i++) {
        char *fld = Iterator_next(di);
        if (i == 1) {
            assert(strncmp(fld, "two", 3) == 0);
        }
    }

    msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 2124);
    cmd = OekMsg_get_get_cmd(msg);
    assert(cmd);
    assert(GetCmd_remove(cmd));
    datalist = GetCmd_get_data_objects(cmd);
    assert(DataObjectList_length(datalist) == 1);
    iter = DataObjectList_iterator(datalist, true);
    data = Iterator_next(iter);
    i = 0;
    for ( Iterator di = DataObject_iterator(data, true); Iterator_hasMore(di); i++) {
        char *fld = Iterator_next(di);
        if (i == 2) {
            assert(strncmp(fld, "mykey2", 6) == 0);
        }
    }

    msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 109);
    cmd = OekMsg_get_con_cmd(msg);
    assert(cmd);

    msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    cmd = OekMsg_get_dis_cmd(msg);
    assert(cmd);

    msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 888);
    assert(OekMsg_get_tid(msg) == 77);
    cmd = OekMsg_get_put_cmd(msg);
    assert(PutCmd_get_dur(cmd) == 999);
    assert(cmd);
    data = PutCmd_get_data(cmd);
    i = 0;
    for ( Iterator di = DataObject_iterator(data, true); Iterator_hasMore(di); i++) {
        char *fld = Iterator_next(di);
        if (i == 2) {
            assert(strncmp(fld, "mykey22", 7) == 0);
        }
    }

    msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 8888);
    cmd = OekMsg_get_sta_cmd(msg);
    assert(cmd);
    assert(StaCmd_get_dur(cmd) == 9999);

    msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 111);
    cmd = OekMsg_get_upt_cmd(msg);
    assert(cmd);

    msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 0);
    cmd = OekMsg_get_upl_cmd(msg);
    assert(cmd);

    msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 1111);
    cmd = OekMsg_get_upt_cmd(msg);
    assert(cmd);

    assert(NULL == OeJclDecoder_next_cmd(decoder, NULL));
    Arena_dispose(arena);

    return 0;
}

int main(void) {


    test_batch_1();

    return 0;
}

