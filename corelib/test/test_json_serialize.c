#include "config.h"
#undef NDEBUG
#include <assert.h>
#include "arena.h"
#include "OekMsg.h"
#include "oec_file.h"
#include "cJSON.h"
#include "json_msg_factory.h"
#include <stdio.h>

static Arena_T arena;

//helpers to bootstrap msg serializing

OekMsg _get_putmapcmd() {

    OekMsg msg = OekMsg_new(PUT_CMD, arena);
    OekMsg_set_cid(msg, 1199);
    OekMsg_set_tid(msg, 1186);
    PutCmd cmd = OekMsg_get_put_cmd(msg);
    DataObject data = PutCmd_get_data(cmd);
    PutCmd_set_dur(cmd, 60000);
    DataObject_add_attr(data, (oe_scalar) "mymapkey");
    DataObject_add_attr(data, (oe_scalar) "mymapval");
    DataObject_add_attr(data, (oe_scalar) "myothermapkey");
    DataObject_add_attr(data, (oe_scalar) "myothermapval");
    return msg;
}

OekMsg _get_putcmd() {

    OekMsg msg = OekMsg_new(PUT_CMD, arena);
    OekMsg_set_cid(msg, 199);
    OekMsg_set_tid(msg, 186);
    PutCmd cmd = OekMsg_get_put_cmd(msg);
    DataObject data = PutCmd_get_data(cmd);
    PutCmd_set_dur(cmd, 60000);
    PutCmd_set_tuple(cmd);
    DataObject_add_attr(data, (oe_scalar) "myputkey1");
    DataObject_add_attr(data, (oe_scalar) "myputkey2");
    char *bytes = "Can you hear me now?"; 
    DataObject_set_nbytes(data, strlen(bytes));
    DataObject_set_bytes(data, bytes);
    return msg;
}

OekMsg _get_putres() {

    OekMsg msg = OekMsg_new(PUT_RES, arena);
    OekMsg_set_cid(msg, 299);
    OekMsg_set_tid(msg, 286);
    PutRes res = OekMsg_get_put_res(msg);
    //PutRes_set_expiretime(res, oec_get_time_milliseconds() + 5000);
    PutRes_set_lease_id(res, 1234);
    return msg;
}

OekMsg _get_getcmd() {

    OekMsg msg = OekMsg_new(GET_CMD, arena);
    OekMsg_set_cid(msg, 99);
    OekMsg_set_tid(msg, 86);
    GetCmd cmd = OekMsg_get_get_cmd(msg);
    GetCmd_set_tuple(cmd);
    GetCmd_set_return_bytes(cmd, true);
    GetCmd_set_remove_option(cmd);
    GetCmd_set_ifexists_option(cmd);
    GetCmd_set_return_attrs(cmd, true);
    //GetCmd_set_max_responses(cmd, 13);
    GetCmd_set_max_responses(cmd, 0);
    GetCmd_set_join(cmd);
    GetCmd_set_timeout(cmd, 5000);
   
    DataObject keys = GetCmd_new_data_object(cmd);
    DataObject_add_attr(keys, (oe_scalar) "mykey1");
    DataObject_add_attr(keys, (oe_scalar) "mykey2");
    keys = GetCmd_new_data_object(cmd);
    DataObject_add_attr(keys, (oe_scalar) "my2key1");
    DataObject_add_attr(keys, (oe_scalar) "my2key2");
    DataObject_add_attr(keys, (oe_scalar) "my2key3");
    
    return msg;
}

OekMsg _get_getres() {

    OekMsg msg = OekMsg_new(GET_RES, arena);
    OekMsg_set_cid(msg, 99);
    OekMsg_set_tid(msg, 186);
    GetRes res = OekMsg_get_get_res(msg);
    DataObject data = GetRes_get_data(res);
    GetRes_set_nresults(res, 1);
   
    DataObject_add_attr(data, (oe_scalar) "yourkey1");
    DataObject_add_attr(data, (oe_scalar) "yourkey2");
    DataObject_add_attr(data, (oe_scalar) "yourkey3");
   
    char *bytes = "123456789"; 
    DataObject_set_nbytes(data, strlen(bytes));
    DataObject_set_bytes(data, bytes);
    return msg;
}

OekMsg _get_stacmd() {
    OekMsg msg = OekMsg_new(STA_CMD, arena);
    OekMsg_set_cid(msg, 100);
    StaCmd cmd = OekMsg_get_sta_cmd(msg);
    StaCmd_set_dur(cmd, 50000);
    return msg;
}
OekMsg _get_stares() {
    OekMsg msg = OekMsg_new(STA_RES, arena);
    OekMsg_set_cid(msg, 100);
    OekMsg_set_tid(msg, 666);
    StaRes res = OekMsg_get_sta_res(msg);
    //StaRes_set_expiretime(res, oec_get_time_milliseconds() + 50000);
    return msg;
}

OekMsg _get_uptcmd() {
    OekMsg msg = OekMsg_new(UPT_CMD, arena);
    OekMsg_set_cid(msg, 101);
    OekMsg_set_tid(msg, 667);
    UptCmd cmd = OekMsg_get_upt_cmd(msg);
    UptCmd_set_dur(cmd, 5000);
    UptCmd_set_status(cmd, 'C');
    UptCmd_set_enlisted(cmd, 99);
    return msg;
}
OekMsg _get_uptres() {
    OekMsg msg = OekMsg_new(UPT_RES, arena);
    OekMsg_set_cid(msg, 101);
    OekMsg_set_tid(msg, 667);
    UptRes res = OekMsg_get_upt_res(msg);
    //UptRes_set_expiretime(res, oec_get_time_milliseconds() + 50000);
    UptRes_set_status(res, 'R');
    return msg;
}

OekMsg _get_uplcmd() {
    OekMsg msg = OekMsg_new(UPL_CMD, arena);
    OekMsg_set_cid(msg, 545);
    UplCmd cmd = OekMsg_get_upl_cmd(msg);
    UplCmd_set_lid(cmd, 123);
    UplCmd_set_dur(cmd, 5000);
    return msg;
}
OekMsg _get_uplres() {
    OekMsg msg = OekMsg_new(UPL_RES, arena);
    OekMsg_set_cid(msg, 102);
    UplRes res = OekMsg_get_upl_res(msg);
    UplRes_set_expiretime(res, oec_get_time_milliseconds() + 50000);
    return msg;
}

OekMsg _get_concmd() {
    OekMsg msg = OekMsg_new(CON_CMD, arena);
    ConCmd cmd = OekMsg_get_con_cmd(msg);
    ConCmd_set_extentname(cmd, (oe_scalar) "myspace");
    ConCmd_set_pwd(cmd, (oe_scalar) "secret");
    ConCmd_set_username(cmd, (oe_scalar) "myname");
    ConCmd_set_version_maj(cmd, 0);
    ConCmd_set_version_min(cmd, 3);
    ConCmd_set_version_upt(cmd, 7);
    return msg;
}
OekMsg _get_conres() {
    OekMsg msg = OekMsg_new(CON_RES, arena);
    ConRes res = OekMsg_get_con_res(msg);
    ConRes_set_extentname(res, (oe_scalar) "myspace");
    ConRes_set_sid(res, 9876);
    ConRes_set_version_maj(res, 0);
    ConRes_set_version_min(res, 3);
    ConRes_set_version_upt(res, 9);
    return msg;
}

OekMsg _get_discmd() {
    OekMsg msg = OekMsg_new(DIS_CMD, arena);
    return msg;
}
OekMsg _get_err() {
    OekMsg msg = OekMsg_new(OEK_ERR, arena);
    OekErr err = OekMsg_get_err(msg);
    OekErr_set_message(err, (oe_scalar) "you loose");
    OekErr_set_type(err, 9986);
    return msg;
}

//begin tests

int test_cjson_create() {

    cJSON *root,*fmt;
	root=cJSON_CreateObject();	
    assert(root);
	cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack B. Nimble"));
	cJSON_AddItemToObject(root, "format", fmt=cJSON_CreateObject());
    assert(fmt);
	cJSON_AddStringToObject(fmt,"type",		"rect");
	cJSON_AddNumberToObject(fmt,"width",		1920);
	cJSON_AddNumberToObject(fmt,"height",		1080);
	cJSON_AddFalseToObject (fmt,"interlace");
	cJSON_AddNumberToObject(fmt,"frame rate",	26);
	char *rendered=cJSON_Print(root);
    assert(rendered);
    printf("new json:%s\n", rendered);
	cJSON_Delete(root);
    return 0;
}

int test_cjson_parse() {

    char *my_json_string = oec_read_file("test1.json");
    printf("file json:\n%s\n", my_json_string);
    cJSON *root = cJSON_Parse(my_json_string);
	char *name = cJSON_GetObjectItem(root,"name")->valuestring;

    printf("file json name:%s\n", name);
	cJSON *format = cJSON_GetObjectItem(root,"format");
	int framerate = cJSON_GetObjectItem(format,"frame rate")->valueint;
    assert(framerate == 24);
    assert( cJSON_GetObjectItem(format,"frame rate")->type != cJSON_NULL);

    cJSON *node = cJSON_GetObjectItem(format,"rame rate");
    assert(!node);

	cJSON_Delete(root);
    return 0;
}

int test_cjson_con_cmd() {

    OekMsg msg = _get_concmd();
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("con json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    ConCmd cmd = OekMsg_get_con_cmd(newmsg);
    assert(!ConCmd_rollback_on_disconnect(cmd));
    printf("new con extentname: %s\n", ConCmd_get_extentname(cmd));

    return 0;
}

int test_cjson_con_cmd_rb_dis() {

    OekMsg msg = _get_concmd();
    ConCmd cmd = OekMsg_get_con_cmd(msg);
    ConCmd_set_rollback_on_disconnect(cmd);
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("con json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    ConCmd newcmd = OekMsg_get_con_cmd(newmsg);
    assert(ConCmd_rollback_on_disconnect(newcmd));
    printf("new con extentname: %s\n", ConCmd_get_extentname(newcmd));

    return 0;
}

int test_cjson_dis_cmd() {

    OekMsg msg = _get_discmd();
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("dis json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    DisCmd cmd = OekMsg_get_dis_cmd(newmsg);

    return 0;
}

int test_cjson_con_res() {

    OekMsg msg = _get_conres();
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("con res json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    ConRes res = OekMsg_get_con_res(newmsg);
    printf("new con res sid: %i\n", ConRes_get_sid(res));

    return 0;
}

int test_cjson_put_map_cmd() {

    OekMsg msg = _get_putmapcmd();
    oe_id sid = OekMsg_get_cid(msg);
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("put map cmd json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    oe_id newsid = OekMsg_get_cid(newmsg);
    assert(newsid);
    assert(newsid == sid);
    PutCmd cmd = OekMsg_get_put_cmd(newmsg);
    printf("new put map cmd dur: %d\n", PutCmd_get_dur(cmd));

    return 0;
}

int test_cjson_put_cmd() {

    OekMsg msg = _get_putcmd();
    oe_id sid = OekMsg_get_cid(msg);
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("put cmd json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    oe_id newsid = OekMsg_get_cid(newmsg);
    assert(newsid);
    assert(newsid == sid);
    PutCmd cmd = OekMsg_get_put_cmd(newmsg);
    printf("new put cmd dur: %d\n", PutCmd_get_dur(cmd));

    return 0;
}

int test_cjson_put_res() {

    OekMsg msg = _get_putres();
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("put res json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    PutRes res = OekMsg_get_put_res(newmsg);
    printf("new put res sid: %i\n", PutRes_get_lease_id(res));

    return 0;
}

int test_cjson_get_cmd() {

    OekMsg msg = _get_getcmd();
    oe_id sid = OekMsg_get_cid(msg);
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("get cmd json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    oe_id newsid = OekMsg_get_cid(newmsg);
    assert(newsid);
    assert(newsid == sid);
    printf("new get cmd cid: %d\n", OekMsg_get_cid(newmsg));
    GetCmd cmd = OekMsg_get_get_cmd(newmsg);
    printf("new get cmd dur: %d\n", GetCmd_get_timeout(cmd));
    assert(GetCmd_is_tuple(cmd));
    assert(GetCmd_get_join(cmd));
    assert(GetCmd_remove(cmd));
    assert(GetCmd_return_attrs(cmd));
    //assert(GetCmd_get_max_responses(cmd) == 13);
    assert(GetCmd_get_max_responses(cmd) == 0);

    return 0;
}

int test_cjson_get_res() {

    OekMsg msg = _get_getres();
    oe_id sid = OekMsg_get_cid(msg);
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("get res json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    oe_id newsid = OekMsg_get_cid(newmsg);
    assert(newsid);
    assert(newsid == sid);
    printf("new get res cid: %d\n", OekMsg_get_cid(newmsg));
    GetRes res = OekMsg_get_get_res(newmsg);
    printf("new get res nresults: %d\n", GetRes_get_nresults(res));

    return 0;
}

int test_cjson_sta_cmd() {

    OekMsg msg = _get_stacmd();
    oe_id cid = OekMsg_get_cid(msg);
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("sta cmd json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    oe_id newcid = OekMsg_get_cid(newmsg);
    printf("new sta cmd cid: %d\n", OekMsg_get_cid(newmsg));
    assert(newcid == cid);
    StaCmd cmd = OekMsg_get_sta_cmd(newmsg);
    printf("new sta cmd dur: %d\n", StaCmd_get_dur(cmd));

    return 0;
}

int test_cjson_sta_res() {

    OekMsg msg = _get_stares();
    oe_id cid = OekMsg_get_cid(msg);
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("sta res json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    oe_id newcid = OekMsg_get_cid(newmsg);
    printf("new sta res cid: %d\n", OekMsg_get_cid(newmsg));
    assert(newcid == cid);
    StaRes res = OekMsg_get_sta_res(newmsg);
    //printf("new sta res exptime: %d\n", StaRes_get_expiretime(res));

    return 0;
}

int test_cjson_upl_cmd() {

    OekMsg msg = _get_uplcmd();
    oe_id cid = OekMsg_get_cid(msg);
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("upl cmd json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    oe_id newcid = OekMsg_get_cid(newmsg);
    printf("new upl cmd cid: %d\n", newcid);
    assert(newcid == cid);
    UplCmd cmd = OekMsg_get_upl_cmd(newmsg);
    oe_dur dur = UplCmd_get_dur(cmd);
    assert(dur == 5000);
    oe_id lid = UplCmd_get_lid(cmd);
    assert(lid == 123);

    return 0;
}

int test_cjson_upl_res() {

    OekMsg msg = _get_uplres();
    oe_id cid = OekMsg_get_cid(msg);
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("upl res json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    oe_id newcid = OekMsg_get_cid(newmsg);
    printf("new upl res cid: %d\n", newcid);
    assert(newcid == cid);
    UplRes res = OekMsg_get_upl_res(newmsg);
    printf("new upl res exptime: %d\n", UplRes_get_expiretime(res));

    return 0;
}

int test_cjson_upt_cmd() {

    OekMsg msg = _get_uptcmd();
    oe_id cid = OekMsg_get_cid(msg);
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("upt cmd json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    oe_id newcid = OekMsg_get_cid(newmsg);
    printf("new upt cmd cid: %d\n", newcid);
    assert(newcid == cid);
    UptCmd cmd = OekMsg_get_upt_cmd(newmsg);
    oe_dur dur = UptCmd_get_dur(cmd);
    assert(dur == 5000);
    char status = UptCmd_get_status(cmd);
    assert(status == 'C');
    int enlisted = UptCmd_get_enlisted(cmd);
    assert(enlisted == 99);

    return 0;
}

int test_cjson_upt_res() {

    OekMsg msg = _get_uptres();
    UptRes res1 = OekMsg_get_upt_res(msg);
    oe_id cid = OekMsg_get_cid(msg);
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("upt res json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    oe_id newcid = OekMsg_get_cid(newmsg);
    printf("new upt res cid: %d\n", newcid);
    assert(newcid == cid);
    UptRes res = OekMsg_get_upt_res(newmsg);
    //printf("new upl res exptime: %d\n", UptRes_get_expiretime(res));
    char status = UptRes_get_status(res);
    printf("new upl res status: %c\n", status);
    assert(status == 'R');

    return 0;
}

int test_cjson_error() {

    OekMsg msg = _get_err();
    OekErr err1 = OekMsg_get_err(msg);
    oe_id cid = OekMsg_get_cid(msg);
    char *str = 0;
    json_msg_serialize(msg, &str, arena);
    assert(str);
    printf("err json:%s\n", str);

    OekMsg newmsg = 0;
    int rc = json_msg_create(&newmsg, str, arena);
    assert(newmsg);
    oe_id newcid = OekMsg_get_cid(newmsg);
    printf("new err cid: %d\n", newcid);
    assert(newcid == cid);
    OekErr err = OekMsg_get_err(newmsg);
    printf("new err code: %i\n", OekErr_get_type(err));
    printf("new err msg: %s\n", OekErr_get_message(err));
    assert(OekErr_get_type(err) == 9986);

    return 0;
}

int main() {

    arena = Arena_new();

    assert(!test_cjson_parse());
    assert(!test_cjson_create());

    assert(!test_cjson_con_cmd());
    assert(!test_cjson_con_res());

    assert(!test_cjson_con_cmd_rb_dis());

    assert(!test_cjson_put_cmd());
    assert(!test_cjson_put_res());

    assert(!test_cjson_get_cmd());
    assert(!test_cjson_get_res());
 
    assert(!test_cjson_sta_cmd());
    assert(!test_cjson_sta_res());
 
    assert(!test_cjson_upl_cmd());
    assert(!test_cjson_upl_res());
 
    assert(!test_cjson_upt_cmd());
    assert(!test_cjson_upt_res());

    assert(!test_cjson_dis_cmd());

    assert(!test_cjson_error());

    assert(!test_cjson_put_map_cmd());

    return 0;
}

