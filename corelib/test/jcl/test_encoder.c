#include "oejcl_parser.h"
#include "assert.h"
#include "oejcl_parser.tab.h"
#include "OeJclDecoder.h"
#include "OeJclEncoder.h"

int test_read(void) {

    printf("----------- read -----------\n");

    Arena_T arena = Arena_new();

    char *input = "read(tid=38 cid=2123 timeout=22 [\"one\" \"two\"]);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);

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

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    printf("test_take text: %s\n", text);
    assert(strncmp("take", text, 4) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 2124);
    assert( OekMsg_get_type(newmsg) == GET_CMD );
    assert(OekMsg_get_tid(newmsg) == 138);

    Arena_dispose(&arena);

    return 0;
}

int test_connect(void) {

    printf("----------- connect -----------\n");
    Arena_T arena = Arena_new();

    char *input = "connect(cid=109 extentname=\"myspace\" username=\"me\" password=\"my\");";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    printf("test_connect text: %s\n", text);
    assert(strncmp("connect", text, 7) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 109);
    assert( OekMsg_get_type(newmsg) == CON_CMD );

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

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    assert(strncmp("dis", text, 3) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert( OekMsg_get_type(newmsg)  == DIS_CMD );

    Arena_dispose(&arena);

    return 0;
}

int test_write(void) {

    printf("----------- write -----------\n");
    Arena_T arena = Arena_new();

    char *input = "write(tid=77 cid=888 dur=999 [\"my11\" \"your11\"]);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    assert(strncmp("write(", text, 6) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 888);
    assert(OekMsg_get_tid(newmsg) == 77);
    assert( OekMsg_get_type(newmsg) == PUT_CMD );

    PutCmd cmd = OekMsg_get_put_cmd(newmsg);
    assert(cmd);
    assert(PutCmd_is_tuple(cmd));
    assert(PutCmd_get_dur(cmd) == 999);

    Arena_dispose(&arena);

    return 0;
}

int test_write_map(void) {

    printf("----------- write -----------\n");
    Arena_T arena = Arena_new();

    char *input = "write(tid=77 cid=888 dur=999 [\"mykey11\": \"myval11\" \"mykey22\" :\"myval22\"]);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    assert(strncmp("write(", text, 6) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 888);
    assert( OekMsg_get_type(newmsg) == PUT_CMD );

    PutCmd cmd = OekMsg_get_put_cmd(newmsg);
    assert(cmd);
    assert(!PutCmd_is_tuple(cmd));

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
    StaCmd cmd = OekMsg_get_sta_cmd(msg);
    assert(StaCmd_get_dur(cmd) == 9999);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    assert(strncmp("txn(", text, 4) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 8888);
    assert( OekMsg_get_type(newmsg) == STA_CMD );

    StaCmd newcmd = OekMsg_get_sta_cmd(newmsg);
    assert(newcmd);
    assert(StaCmd_get_dur(newcmd) == 9999);

    Arena_dispose(&arena);

    return 0;
}

int test_commit(void) {

    printf("----------- commit -----------\n");
    Arena_T arena = Arena_new();

    char *input = "commit(cid=111 tid=8 enlisted=1);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    assert(strncmp("commit(", text, 7) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 111);
    assert(OekMsg_get_tid(newmsg) == 8);
    assert( OekMsg_get_type(newmsg) == UPT_CMD );

    UptCmd cmd = OekMsg_get_upt_cmd(newmsg);
    assert(cmd);
    assert(UptCmd_get_status(cmd) == OETXN_COMMITTED);

    Arena_dispose(&arena);

    return 0;
}

int test_cancel(void) {

    printf("----------- cancel -----------\n");
    Arena_T arena = Arena_new();

    char *input = "cancel(cid=1111 tid=99);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    assert(strncmp("cancel(", text, 7) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 1111);
    assert(OekMsg_get_tid(newmsg) == 99);
    assert( OekMsg_get_type(newmsg) == UPT_CMD );

    UptCmd cmd = OekMsg_get_upt_cmd(newmsg);
    assert(cmd);
    assert(UptCmd_get_status(cmd) == OETXN_ROLLEDBACK);

    Arena_dispose(&arena);

    return 0;
}

int test_write_res(void) {

    printf("----------- write res -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=write cid=1 lid=9);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    assert(strncmp("response(", text, 9) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 1);
    assert( OekMsg_get_type(newmsg) == PUT_RES );

    PutRes res = OekMsg_get_put_res(newmsg);
    assert(res);
    assert(PutRes_get_lease_id(res) == 9);

    Arena_dispose(&arena);

    return 0;
}

int test_read_res(void) {

    printf("----------- read res -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=read cid=1888 [\"my111\" \"your112\"]);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    assert(strncmp("response(", text, 9) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 1888);
    assert( OekMsg_get_type(newmsg) == GET_RES );

    GetRes res = OekMsg_get_get_res(newmsg);
    assert(res);
    assert(GetRes_is_tuple(res) == true);
    assert(GetRes_is_destroyed(res) == false);

    Arena_dispose(&arena);

    return 0;
}

int test_take_res(void) {

    printf("----------- take res -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=take           cid=91888 [\"mykey1\": \"myval1\"]);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    assert(strncmp("response(", text, 9) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 91888);
    assert( OekMsg_get_type(newmsg) == GET_RES );

    GetRes res = OekMsg_get_get_res(newmsg);
    assert(res);
    assert(GetRes_is_tuple(res) == false);
    assert(GetRes_is_destroyed(res) == true);

    Arena_dispose(&arena);

    return 0;
}

int test_con_res(void) {

    printf("----------- con res -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=connect cid=200 sid=123456);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);
    assert(decoder);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    assert(strncmp("response(", text, 9) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 200);
    assert( OekMsg_get_type(newmsg) == CON_RES );

    ConRes res = OekMsg_get_con_res(newmsg);
    assert(res);
    oe_id sid = ConRes_get_sid(res);
    assert(sid == 123456);

    Arena_dispose(&arena);

    return 0;
}

int test_commit_res(void) {

    printf("----------- commit res -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=commit cid=201);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    assert(strncmp("response(", text, 9) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 201);
    assert( OekMsg_get_type(newmsg) == UPT_RES );

    UptRes res = OekMsg_get_upt_res(newmsg);
    assert(res);
    assert(UptRes_get_status(res) == 'C');

    Arena_dispose(&arena);

    return 0;
}

int test_cancel_res(void) {

    printf("----------- cancel res -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=cancel cid=202);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    assert(strncmp("response(", text, 9) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 202);
    assert( OekMsg_get_type(newmsg) == UPT_RES );

    UptRes res = OekMsg_get_upt_res(newmsg);
    assert(res);
    assert(UptRes_get_status(res) == 'R');

    Arena_dispose(&arena);

    return 0;
}

int test_begin_res(void) {

    printf("----------- txn res -----------\n");
    Arena_T arena = Arena_new();

    char *input = "response(cmd=txn cid=203 tid=999);";
    OeJclDecoder decoder = OeJclDecoder_new(arena, input);

    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    assert(msg);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *text = OeJclEncoder_msg_serialize(encoder, msg);
    assert(text);
    assert(strncmp("response(", text, 9) == 0);

    OeJclDecoder newdecoder = OeJclDecoder_new(arena, text);
    OekMsg newmsg = OeJclDecoder_next_cmd(newdecoder, NULL);
    assert(OekMsg_get_cid(newmsg) == 203);
    assert(OekMsg_get_tid(newmsg) != NULL);
    assert( OekMsg_get_type(newmsg) == STA_RES );

    StaRes res = OekMsg_get_sta_res(newmsg);
    assert(res);

    Arena_dispose(&arena);

    return 0;
}

int main(void) {


    test_connect();
    test_disconnect();
    test_write();
    test_write_map();
    test_read();
    test_take();
    test_begin();
    test_commit();
    test_cancel();

    //responses
    test_write_res();
    test_read_res();
    test_take_res();
    test_con_res();
    test_commit_res();
    test_cancel_res();
    test_begin_res();

    return 0;
}

