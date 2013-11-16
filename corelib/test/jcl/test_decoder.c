#include "oejcl_parser.h"
#include "assert.h"
#include "oejcl_parser.tab.h"
#include "OeJclDecoder.h"

int test_read(void) {

    printf("----------- read -----------\n");

    Arena_T arena = Arena_new();

    char *input = "read(tid=38 cid=2123 timeout=22 [\"one\" \"two\"]);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 2123);
    GetCmd cmd = OekMsg_get_get_cmd(msg);
    assert(cmd);
    assert(!GetCmd_remove(cmd));

    DataObjectList datalist = GetCmd_get_data_objects(cmd);
    assert(DataObjectList_length(datalist) == 1);
    Iterator iter = DataObjectList_iterator(datalist, true);
    DataObject data = Iterator_next(iter);
    int i = 0;
    for ( Iterator di = DataObject_iterator(data, true); Iterator_hasMore(di); i++) {
        char *fld = Iterator_next(di);
        if (i == 0) {
            assert(strncmp(fld, "one", 3) == 0);
        }
        if (i == 1) {
            assert(strncmp(fld, "two", 3) == 0);
        }
    }

    Arena_dispose(&arena);

    return 0;
}

int test_take(void) {

    printf("----------- take -----------\n");
    Arena_T arena = Arena_new();

    char *input = "take(tid=138 cid=2124 timeout=122 [\"mykey1\": \"myval1\" \"mykey2\" :\"myval2\"]);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 2124);
    GetCmd cmd = OekMsg_get_get_cmd(msg);
    assert(cmd);
    assert(GetCmd_remove(cmd));
    DataObjectList datalist = GetCmd_get_data_objects(cmd);
    assert(DataObjectList_length(datalist) == 1);
    Iterator iter = DataObjectList_iterator(datalist, true);
    DataObject data = Iterator_next(iter);
    int i = 0;
    for ( Iterator di = DataObject_iterator(data, true); Iterator_hasMore(di); i++) {
        char *fld = Iterator_next(di);
        if (i == 2) {
            assert(strncmp(fld, "mykey2", 6) == 0);
        }
    }

    Arena_dispose(&arena);

    return 0;
}

int test_connect(void) {

    printf("----------- connect -----------\n");
    Arena_T arena = Arena_new();

    char *input = "connect(cid=109);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 109);
    ConCmd cmd = OekMsg_get_con_cmd(msg);
    assert(cmd);

    Arena_dispose(&arena);

    return 0;
}

int test_disconnect(void) {

    printf("----------- disconnect -----------\n");
    Arena_T arena = Arena_new();

    char *input = "dis();";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    DisCmd cmd = OekMsg_get_dis_cmd(msg);
    assert(cmd);

    Arena_dispose(&arena);

    return 0;
}

int test_write(void) {

    printf("----------- write -----------\n");
    Arena_T arena = Arena_new();

    char *input = "write(tid=77 cid=888 dur=999 [\"mykey11\": \"myval11\" \"mykey22\" :\"myval22\"]);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 888);
    assert(OekMsg_get_tid(msg) == 77);
    PutCmd cmd = OekMsg_get_put_cmd(msg);
    assert(PutCmd_get_dur(cmd) == 999);
    assert(cmd);
    DataObject data = PutCmd_get_data(cmd);
    int i = 0;
    for ( Iterator di = DataObject_iterator(data, true); Iterator_hasMore(di); i++) {
        char *fld = Iterator_next(di);
        if (i == 2) {
            assert(strncmp(fld, "mykey22", 7) == 0);
        }
    }

    Arena_dispose(&arena);

    return 0;
}

int test_begin(void) {

    printf("----------- begin -----------\n");
    Arena_T arena = Arena_new();

    char *input = "txn(cid=8888 timeout=9999);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 8888);
    StaCmd cmd = OekMsg_get_sta_cmd(msg);
    assert(cmd);
    assert(StaCmd_get_dur(cmd) == 9999);

    Arena_dispose(&arena);

    return 0;
}

int test_commit(void) {

    printf("----------- commit -----------\n");
    Arena_T arena = Arena_new();

    char *input = "commit(cid=111 tid=99 enlisted=1);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 111);
    UptCmd cmd = OekMsg_get_upt_cmd(msg);
    assert(cmd);
    assert(UptCmd_get_status(cmd) == OETXN_COMMITTED);

    Arena_dispose(&arena);

    return 0;
}

int test_cancel(void) {

    printf("----------- cancel -----------\n");
    Arena_T arena = Arena_new();

    char *input = "cancel(cid=1111 tid=222);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    UptCmd cmd = OekMsg_get_upt_cmd(msg);
    assert(OekMsg_get_cid(msg) == 1111);
    assert(cmd);
    assert(UptCmd_get_status(cmd) == OETXN_ROLLEDBACK);

    Arena_dispose(&arena);

    return 0;
}

int test_connect_res(void) {

    printf("----------- connect response -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=connect cid=1111);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 1111);
    ConRes res = OekMsg_get_con_res(msg);
    assert(res);

    Arena_dispose(&arena);

    return 0;
}

int test_write_res(void) {

    printf("----------- write response -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=write cid=1211 lid=888);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 1211);
    PutRes res = OekMsg_get_put_res(msg);
    assert(res);

    Arena_dispose(&arena);

    return 0;
}

int test_read_res(void) {

    printf("----------- read response -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=read cid=1221 [\"one\" \"tw0\" \"three\"]);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 1221);
    GetRes res = OekMsg_get_get_res(msg);
    DataObject rdata = GetRes_get_data(res);
    assert(DataObject_get_nattrs(rdata) == 3);
    Iterator iter = DataObject_iterator(rdata, true);
    Iterator_next(iter); //go to two
    assert(strncmp("tw0", Iterator_next(iter), 3) == 0);
    assert(res);

    Arena_dispose(&arena);

    return 0;
}

int test_take_res(void) {

    printf("----------- take response -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=take cid=2221 [\"ones\" \"tw0s\" \"threes\" \"fours\"]);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 2221);
    GetRes res = OekMsg_get_get_res(msg);
    DataObject rdata = GetRes_get_data(res);
    assert(DataObject_get_nattrs(rdata) == 4);
    Iterator iter = DataObject_iterator(rdata, true);
    Iterator_next(iter); //go to two
    assert(strncmp("tw0s", Iterator_next(iter), 4) == 0);
    assert(res);

    Arena_dispose(&arena);

    return 0;
}

int test_begin_res(void) {

    printf("----------- txn response -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=txn cid=91211 tid=1888);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 91211);
    assert(OekMsg_get_tid(msg) == 1888);
    StaRes res = OekMsg_get_sta_res(msg);
    assert(res);

    Arena_dispose(&arena);

    return 0;
}

int test_commit_res(void) {

    printf("----------- commit response -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=commit cid=91212);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 91212);
    UptRes res = OekMsg_get_upt_res(msg);
    assert(res);
    assert(UptRes_get_status(res) == OETXN_COMMITTED);

    Arena_dispose(&arena);

    return 0;
}

int test_cancel_res(void) {

    printf("----------- cancel response -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=cancel cid=31212);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 31212);
    UptRes res = OekMsg_get_upt_res(msg);
    assert(res);
    assert(UptRes_get_status(res) == OETXN_ROLLEDBACK);

    Arena_dispose(&arena);

    return 0;
}

int test_err(void) {

    printf("----------- err -----------\n");
    Arena_T arena = Arena_new();

    char *input = "error(cmd=commit cid=12 msg=\"die you bstd die\");";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 12);
    OekErr err = OekMsg_get_err(msg);
    assert(err);

    Arena_dispose(&arena);

    return 0;

}
//response(cmd=read cid=27 nresults=0 []);
int test_empty_read(void) {

    printf("----------- empty read -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=read cid=27 nresults=0 []);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 27);
    GetRes res = OekMsg_get_get_res(msg);
    assert(res);
    assert(GetRes_get_nresults(res) == 0);

    Arena_dispose(&arena);

    return 0;

}

int test_assign(void) {

    printf("----------- assign -----------\n");
    Arena_T arena = Arena_new();

    char *input = "diemsg = \"die you bstd die\";";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(!msg);

    Arena_dispose(&arena);

    return 0;

}

int test_assign_used_by_write(void) {


    printf("----------- assign with write -----------\n");

    Arena_T arena = Arena_new();

    char *input = "diemsg = \"die you bstd die\"; write(tid=38 cid=2123 dur=22 [\"msg\" diemsg]);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);
    assert(OekMsg_get_cid(msg) == 2123);
    PutCmd cmd = OekMsg_get_put_cmd(msg);
    assert(cmd);

    DataObject data = PutCmd_get_data(cmd);
    int i = 0;
    for ( Iterator di = DataObject_iterator(data, true); Iterator_hasMore(di); i++) {
        char *fld = Iterator_next(di);
        printf("write i: %i fld: %s\n", i, fld);
        if (i == 0) {
            assert(strncmp(fld, "msg", 3) == 0);
        }
    }

    Arena_dispose(&arena);

    return 0;
}

int test_assign_with_read(void) {


    printf("----------- assign with read -----------\n");

    Arena_T arena = Arena_new();

    char *result = "response(cmd=read cid=211 [\"myname\" \"ed\" \"mynickname\" \"eddie\"]);";
    OeJclDecoder rdecoder = OeJclDecoder_new(arena, result);
    OekMsg resmsg = OeJclDecoder_next_cmd(rdecoder, NULL); //should return the result msg
    assert(resmsg);

    char *input = "read(cid=212 timeout=505 [\"_\" mynamevar \"mynickname\" \"eddie\"]); write(cid=213 dur=9909 [\"to\" mynamevar \"msg\" \"hiya\"]);";

    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL); //should return the read cmd
    assert(msg);
    assert(OekMsg_get_cid(msg) == 212);
    GetCmd cmd = OekMsg_get_get_cmd(msg);
    assert(cmd);
    assert(GetCmd_is_tuple(cmd));

    msg = OeJclDecoder_next_cmd(decoder, resmsg); //should return the write cmd
    assert(msg);
    assert(OekMsg_get_cid(msg) == 213);

    PutCmd pcmd = OekMsg_get_put_cmd(msg);
    assert(PutCmd_get_dur(pcmd) == 9909);
    assert(PutCmd_is_tuple(pcmd));
    DataObject data = PutCmd_get_data(pcmd);
    assert(DataObject_get_nattrs(data) == 4);
    int i = 0;
    for ( Iterator di = DataObject_iterator(data, true); Iterator_hasMore(di); i++) {
        char *fld = Iterator_next(di);
        printf("write i: %i fld: %s\n", i, fld);
        if (i == 1) {
            assert(strncmp(fld, "ed", 2) == 0);
        }
        if (i == 3) {
            assert(strncmp(fld, "hiya", 4) == 0);
        }
    }

    Arena_dispose(&arena);

    return 0;
}

int test_assign_with_read_map(void) {


    printf("----------- assign with read -----------\n");

    Arena_T arena = Arena_new();

    char *result = "response(cmd=read cid=211 [\"myname\":\"fred\" \"mynickname\":\"freddie\"]);";
    OeJclDecoder rdecoder = OeJclDecoder_new(arena, result);
    OekMsg resmsg = OeJclDecoder_next_cmd(rdecoder, NULL); //should return the result msg
    assert(resmsg);

    char *input = "read(cid=212 timeout=505 [\"myname\": mynamevar \"mynickname\":\"freddie\"]); write(cid=213 dur=9909 [\"to\": mynamevar \"msg\": \"hiyo\"]);";

    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL); //should return the read cmd
    assert(msg);
    assert(OekMsg_get_cid(msg) == 212);
    GetCmd gcmd = OekMsg_get_get_cmd(msg);
    assert(!GetCmd_is_tuple(gcmd));
    assert(gcmd);

    msg = OeJclDecoder_next_cmd(decoder, resmsg); //should return the write cmd
    assert(msg);
    assert(OekMsg_get_cid(msg) == 213);

    PutCmd pcmd = OekMsg_get_put_cmd(msg);
    assert(PutCmd_get_dur(pcmd) == 9909);
    assert(!PutCmd_is_tuple(pcmd));
    DataObject data = PutCmd_get_data(pcmd);
    assert(data);
    assert(DataObject_get_nattrs(data) == 4);
    int i = 0;
    for ( Iterator di = DataObject_iterator(data, true); Iterator_hasMore(di); i++) {
        char *fld = Iterator_next(di);
        printf("write i: %i fld: %s\n", i, fld);
        if (i == 1) {
            assert(strncmp(fld, "fred", 2) == 0);
        }
        if (i == 3) {
            assert(strncmp(fld, "hiyo", 4) == 0);
        }
    }

    Arena_dispose(&arena);

    return 0;
}

int main(void) {


    test_connect();
    test_disconnect();
    test_write();
    test_read();
    test_take();
    test_begin();
    test_commit();
    test_cancel();

    test_connect_res();
    test_write_res();
    test_read_res();
    test_take_res();
    test_begin_res();
    test_commit_res();
    test_cancel_res();
    test_err();

    test_empty_read();

    test_assign();
    test_assign_used_by_write();
    test_assign_with_read();
    test_assign_with_read_map();

    return 0;
}

