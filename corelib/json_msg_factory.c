/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include "mem.h"
#include "oec_log.h"
#include "arena.h"
#include "Iterator.h"
#include <assert.h>
#include "json_msg_factory.h"
#include "cJSON.h"
#include "oescript_core_resources.h"
#include "modp_b64.h"

//todo: add arena support to json lib

static char *_arena_cpy(OekMsg msg, char *str)
{
    size_t len;
    char *copy;

    len = strlen(str) + 1;
    if (!(copy = (char *) Arena_alloc(OekMsg_get_arena(msg), len, __FILE__, __LINE__))) return 0;
    memcpy(copy,str,len);
    return copy;
}

static int _con_command_create(OekMsg msg, cJSON *root) {

    cJSON *valuenode;
    valuenode = cJSON_GetObjectItem(root, OEJSON_CID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number) {
            return -1;
        }
        OekMsg_set_cid( msg, valuenode->valueint);
    }

    ConCmd cmd = OekMsg_get_con_cmd(msg);

    cJSON *versionnode = cJSON_GetObjectItem(root, OEJSON_VERSION);
    if (versionnode) {
        valuenode = cJSON_GetObjectItem(versionnode, OEJSON_MAJ);
        if (valuenode) {
            if (valuenode->type != cJSON_Number)  return -2;
            ConCmd_set_version_maj( cmd, valuenode->valueint);
        }

        valuenode = cJSON_GetObjectItem(versionnode, OEJSON_MIN);
        if (valuenode) {
            if (valuenode->type != cJSON_Number)  return -3;
            ConCmd_set_version_min( cmd, valuenode->valueint);
        }

        valuenode = cJSON_GetObjectItem(versionnode, OEJSON_SEQ);
        if (valuenode) {
            if (valuenode->type != cJSON_Number)  return -4;
            ConCmd_set_version_upt( cmd, valuenode->valueint);
        }
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_EXTENTNAME);
    if (valuenode) {
        if (valuenode->type != cJSON_String)  return -5;
        ConCmd_set_extentname( cmd, _arena_cpy(msg, valuenode->valuestring));
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_USERNAME);
    if (valuenode) {
        if (valuenode->type != cJSON_String)  return -6;
        ConCmd_set_username( cmd, _arena_cpy(msg, valuenode->valuestring));
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_PASSWORD);
    if (valuenode) {
        if (valuenode->type != cJSON_String)  return -7;
        ConCmd_set_pwd( cmd, _arena_cpy(msg, valuenode->valuestring));
    }
    valuenode = cJSON_GetObjectItem(root, "rbOnDisconnect");
    if (valuenode) {
        ConCmd_set_rollback_on_disconnect(cmd);
    }

    return 0;
}

static int _put_command_create(OekMsg msg, cJSON *root) {

    PutCmd cmd = OekMsg_get_put_cmd(msg);
    DataObject data = PutCmd_get_data(cmd);
    DataObject_set_always_copy(data, true);

    cJSON *valuenode;

    valuenode = cJSON_GetObjectItem(root, OEJSON_CID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -8;
        OekMsg_set_cid( msg, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_TID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -9;
        OekMsg_set_tid( msg, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_DUR);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -10;
        PutCmd_set_dur( cmd, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_BYTES);
    if (valuenode) {
        if (valuenode->type != cJSON_String)  return -11;
        int sz = strlen(valuenode->valuestring);
        size_t len = modp_b64_decode_len(sz);
        if (len <= 0) return -12;
        char *bytes = Arena_alloc(OekMsg_get_arena(msg), len, __FILE__, __LINE__);
        int rlen = modp_b64_decode(bytes, (char *) valuenode->valuestring, sz);
        if (rlen <= 0) return -13;
        DataObject_set_nbytes(data, rlen);
        DataObject_set_bytes(data, bytes);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_VALUES);
    if (valuenode) {

        if (valuenode->type == cJSON_Array) {
            PutCmd_set_tuple(cmd);
            int sz = cJSON_GetArraySize(valuenode);
            for (int i = 0; i < sz; i++) {
                cJSON *arraymember = cJSON_GetArrayItem(valuenode, i);
                if (arraymember->type == cJSON_String)
                    DataObject_add_attr( data, arraymember->valuestring);
                else if (arraymember->type == cJSON_Number) {
                    char *strnum = oec_itoa(NULL, arraymember->valueint);
                    DataObject_add_attr( data, strnum);
                    Mem_free(strnum, __FILE__, __LINE__);
                } else {
                    return -20;
                }
            }

        } else if (valuenode->type == cJSON_Object) {

            cJSON *child = valuenode->child;
            while (child) {
                DataObject_add_attr( data, child->string);
                if (child->type == cJSON_String)
                    DataObject_add_attr( data, child->valuestring);
                else if (child->type == cJSON_Number) {
                    char *strnum = oec_itoa(NULL, child->valueint);
                    DataObject_add_attr( data, strnum);
                    Mem_free(strnum, __FILE__, __LINE__);
                } else {
                    return -20;
                }
                child = child->next;
            }

        } else {
            return -14;
        }
    }

    return 0;
}

static int _get_command_create(OekMsg msg, cJSON *root) {

    GetCmd cmd = OekMsg_get_get_cmd(msg);

    cJSON *valuenode;

    valuenode = cJSON_GetObjectItem(root, OEJSON_CID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -15;
        OekMsg_set_cid( msg, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_TID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -16;
        OekMsg_set_tid( msg, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_TIMEOUT);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -17;
        GetCmd_set_timeout( cmd, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_MAXRESULTS);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -18;
        GetCmd_set_max_responses( cmd, valuenode->valueint);
    } else {
        GetCmd_set_max_responses( cmd, 1);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_TEMPLATES);
    if (valuenode) {
        if (valuenode->type != cJSON_Array)  return -19;
        int sz = cJSON_GetArraySize(valuenode);
        if (sz <= 0) {
            return -191;
        }
        for (int i = 0; i < sz; i++) {
            cJSON *jtemplate = cJSON_GetArrayItem(valuenode, i);

            DataObject templt = GetCmd_new_data_object(cmd);
            DataObject_set_always_copy(templt, true);

            //test for array/tuple or object/map
            if (jtemplate->type == cJSON_Array) {
                GetCmd_set_tuple(cmd);
                int msz = cJSON_GetArraySize(jtemplate);
                if (msz <= 0) return -192;
                for (int j = 0; j < msz; j++) {
                    cJSON *arraymember = cJSON_GetArrayItem(jtemplate, j);
                    if (arraymember->type == cJSON_String)
                        DataObject_add_attr( templt, arraymember->valuestring);
                    else if (arraymember->type == cJSON_Number) {
                        char *strnum = oec_itoa(NULL, arraymember->valueint);
                        DataObject_add_attr( templt, strnum);
                        Mem_free(strnum, __FILE__, __LINE__);
                    } else {
                        return -20;
                    }
                }
            } else if (jtemplate->type == cJSON_Object) {
                cJSON *child = jtemplate->child;
                while (child) {
                    DataObject_add_attr( templt, child->string);
                    if (child->type == cJSON_String)
                        DataObject_add_attr( templt, child->valuestring);
                    else if (child->type == cJSON_Number) {
                        char *strnum = oec_itoa(NULL, child->valueint);
                        DataObject_add_attr( templt, strnum);
                        Mem_free(strnum, __FILE__, __LINE__);
                    } else {
                        return -20;
                    }
                    child = child->next;
                }

            } else {
                return -192;
            }

        }
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_ISJOIN);
    if (valuenode)
        if (valuenode->type == cJSON_True) {
            GetCmd_set_join(cmd);
        }

    valuenode = cJSON_GetObjectItem(root, OEJSON_IFEXISTS);
    if (valuenode)
        if (valuenode->type == cJSON_True) {
            GetCmd_set_ifexists_option(cmd);
        }

    valuenode = cJSON_GetObjectItem(root, OEJSON_REMOVE);
    if (valuenode)
        if (valuenode->type == cJSON_True) {
            GetCmd_set_remove_option(cmd);
        }

    valuenode = cJSON_GetObjectItem(root, OEJSON_RETURN_ATTRS);
    if (valuenode)
        if (valuenode->type == cJSON_True) {
            GetCmd_set_return_attrs(cmd, true);
        }

    valuenode = cJSON_GetObjectItem(root, OEJSON_RETURN_BYTES);
    if (valuenode)
        if (valuenode->type == cJSON_True) {
            GetCmd_set_return_bytes(cmd, true);
        }

    return 0;
}

static int _sta_command_create(OekMsg msg, cJSON *root) {

    StaCmd cmd = OekMsg_get_sta_cmd(msg);

    cJSON *valuenode;

    valuenode = cJSON_GetObjectItem(root, OEJSON_CID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -21;
        OekMsg_set_cid( msg, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_DUR);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -22;
        StaCmd_set_dur( cmd, valuenode->valueint);
    }

    return 0;
}

static int _upl_command_create(OekMsg msg, cJSON *root) {

    UplCmd cmd = OekMsg_get_upl_cmd(msg);

    cJSON *valuenode;

    valuenode = cJSON_GetObjectItem(root, OEJSON_CID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -23;
        OekMsg_set_cid( msg, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_DUR);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -24;
        UplCmd_set_dur( cmd, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_LID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -25;
        UplCmd_set_lid( cmd, valuenode->valueint);
    }

    return 0;
}

static int _upt_command_create(OekMsg msg, cJSON *root) {

    UptCmd cmd = OekMsg_get_upt_cmd(msg);

    cJSON *valuenode;

    valuenode = cJSON_GetObjectItem(root, OEJSON_CID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -27;
        OekMsg_set_cid( msg, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_TID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -28;
        OekMsg_set_tid( msg, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_DUR);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -29;
        UptCmd_set_dur( cmd, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_STATUS);
    if (valuenode) {
        if (valuenode->type != cJSON_String)  return -30;
        char *sstr = valuenode->valuestring;
        UptCmd_set_status( cmd, sstr[0]);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_ENLISTED);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -31;
        UptCmd_set_enlisted( cmd, valuenode->valueint);
    }

    return 0;
}

static int _error_create(OekMsg *msgp, cJSON *root, Arena_T arena) {

    OekMsg msg = OekMsg_new(OEK_ERR, arena);
    OekErr err = OekMsg_get_err(msg);

    cJSON *valuenode = cJSON_GetObjectItem(root, OEJSON_MESSAGE);

    if (valuenode) {
        if (valuenode->type != cJSON_String)  return -32;
        OekErr_set_message( err, _arena_cpy(msg, valuenode->valuestring));
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_EID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -33;
        OekErr_set_type( err, valuenode->valueint);
    }

    *msgp = msg;
    return 0;
}

static int _dis_command_create(OekMsg msg, cJSON *root) {

    DisCmd cmd = OekMsg_get_dis_cmd(msg);

    //noop

    return 0;
}

static int _con_response_create(OekMsg msg, cJSON *root) {

    cJSON *valuenode;
    valuenode = cJSON_GetObjectItem(root, OEJSON_CID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -34;
        OekMsg_set_cid( msg, valuenode->valueint);
    }

    ConRes res = OekMsg_get_con_res(msg);

    cJSON *versionnode = cJSON_GetObjectItem(root, OEJSON_VERSION);
    if (versionnode) {
        if (versionnode->type != cJSON_Object)  return -35;
        valuenode = cJSON_GetObjectItem(versionnode, OEJSON_MAJ);
        if (valuenode) {
            if (valuenode->type != cJSON_Number)  return -36;
            ConRes_set_version_maj( res, valuenode->valueint);
        }

        valuenode = cJSON_GetObjectItem(versionnode, OEJSON_MIN);
        if (valuenode) {
            if (valuenode->type != cJSON_Number)  return -37;
            ConRes_set_version_min( res, valuenode->valueint);
        }

        valuenode = cJSON_GetObjectItem(versionnode, OEJSON_SEQ);
        if (valuenode) {
            if (valuenode->type != cJSON_Number)  return -38;
            ConRes_set_version_upt( res, valuenode->valueint);
        }
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_EXTENTNAME);
    if (valuenode) {
        if (valuenode->type != cJSON_String)  return -39;
        ConRes_set_extentname( res, _arena_cpy(msg, valuenode->valuestring));
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_SID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -40;
        ConRes_set_sid( res, valuenode->valueint);
    }

    return 0;
}

static int _put_response_create(OekMsg msg, cJSON *root) {

    PutRes res = OekMsg_get_put_res(msg);

    cJSON *valuenode;

    /*
    valuenode = cJSON_GetObjectItem(root, OEJSON_EXPTIME);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -1;
        PutRes_set_expiretime( res, valuenode->valueint);
    }
    */

    valuenode = cJSON_GetObjectItem(root, OEJSON_LID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -42;
        PutRes_set_lease_id( res, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_CID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -43;
        OekMsg_set_cid(msg, valuenode->valueint);
    }

    return 0;
}

static int _get_response_create(OekMsg msg, cJSON *root) {

    GetRes res = OekMsg_get_get_res(msg);
    DataObject data = GetRes_get_data(res);
    DataObject_set_always_copy(data, true);

    cJSON *valuenode, *jvalues, *item;

    valuenode = cJSON_GetObjectItem(root, OEJSON_CID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -44;
        OekMsg_set_cid(msg, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_NRESULTS);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -45;
        GetRes_set_nresults( res, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_RETURN_BYTES);
    if (valuenode)
        if (valuenode->type == cJSON_True) {
            GetRes_set_destroyed(res);
        }

    jvalues = cJSON_GetObjectItem(root, OEJSON_VALUES);
    if (jvalues) {
        if (jvalues->type == cJSON_Array) {
            GetRes_set_tuple(res);
            int sz = cJSON_GetArraySize(jvalues);
            for (int i = 0; i < sz; i++) {
                item = cJSON_GetArrayItem(jvalues, i);
                if (item) {
                    if (item->type != cJSON_String)  return -47;
                    DataObject_add_attr( data, item->valuestring);
                }
            }
        } else if (jvalues->type == cJSON_Object) {
            cJSON *child = jvalues->child;
            while (child) {
                DataObject_add_attr( data, child->string);
                if (child->type == cJSON_String)
                    DataObject_add_attr( data, child->valuestring);
                else if (child->type == cJSON_Number) {
                    char *strnum = oec_itoa(NULL, child->valueint);
                    DataObject_add_attr( data, strnum);
                    Mem_free(strnum, __FILE__, __LINE__);
                } else {
                    return -20;
                }
                child = child->next;
            }
        } else {
            return -46;
        }
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_BYTES);
    if (valuenode) {
        if (valuenode->type != cJSON_String)  return -48;
        DataObject_set_nbytes(data, strlen(valuenode->valuestring));
        DataObject_set_bytes(data, valuenode->valuestring);
    }

    return 0;
}

static int _sta_response_create(OekMsg msg, cJSON *root) {

    GetRes res = OekMsg_get_sta_res(msg);

    cJSON *valuenode;

    valuenode = cJSON_GetObjectItem(root, OEJSON_TID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -49;
        OekMsg_set_tid(msg, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_CID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -50;
        OekMsg_set_cid(msg, valuenode->valueint);
    }

    /*
    valuenode = cJSON_GetObjectItem(root, OEJSON_EXPTIME);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -1;
        StaRes_set_expiretime( res, valuenode->valueint);
    }
    */

    return 0;
}

static int _upl_response_create(OekMsg msg, cJSON *root) {

    UplRes res = OekMsg_get_upl_res(msg);

    cJSON *valuenode;

    valuenode = cJSON_GetObjectItem(root, OEJSON_CID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -52;
        OekMsg_set_cid(msg, valuenode->valueint);
    }

    valuenode = cJSON_GetObjectItem(root, OEJSON_EXPTIME);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -53;
        UplRes_set_expiretime( res, valuenode->valueint);
    }

    return 0;
}

static int _upt_response_create(OekMsg msg, cJSON *root) {

    UptRes res = OekMsg_get_upt_res(msg);

    cJSON *valuenode;

    valuenode = cJSON_GetObjectItem(root, OEJSON_CID);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -54;
        OekMsg_set_cid(msg, valuenode->valueint);
    }

    /*
    valuenode = cJSON_GetObjectItem(root, OEJSON_EXPTIME);
    if (valuenode) {
        if (valuenode->type != cJSON_Number)  return -1;
        UptRes_set_expiretime( res, valuenode->valueint);
    }
    */

    valuenode = cJSON_GetObjectItem(root, OEJSON_STATUS);
    if (valuenode) {
        if (valuenode->type != cJSON_String)  return -56;
        char *sstr = valuenode->valuestring;
        UptRes_set_status( res, sstr[0]);
    }

    return 0;
}

/*

//ERR corid:id type:string message:string
static int _error_create(OekMsg msg, unsigned char **tokens) {

    if (!isValid(tokens, -1, "ERR")) {
        return -1;
    }
    OekErr err = OekMsg_get_err(msg);

    if (!is_number((char *) tokens[1])) return -1;
    OekMsg_set_cid( msg, atoll((char *) tokens[1]) );
    if (!is_number((char *) tokens[2])) return -1;
    OekErr_set_type( err, atoll((char *) tokens[2]) );
    OekErr_set_message( err, (oe_chars) tokens[3] );

    return 0;
}

*/

//
//serialize
//

static char* _get_json_string(cJSON* root, Arena_T arena) {
    char *str = cJSON_Print(root);
    //ugh, need to memcpy because cJSON won't play arena
    int len = strlen(str);
    char *strcp = Arena_alloc(arena, len+1, __FILE__, __LINE__);
    memcpy(strcp, str, len+1);
    //strcp[len] = '\n';
    //strcp[len+1] = '\n';
    //strcp[len+2] = NULL;
    Mem_free(str, __FILE__, __LINE__);
    return strcp;
}

static int _con_command_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    ConCmd cmd = OekMsg_get_con_cmd(msg);

    cJSON *root, *ver;
    root=cJSON_CreateObject();  
    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_COMMAND);
    cJSON_AddStringToObject(root, OEJSON_NAME, OEJSON_CON);
    if (ConCmd_rollback_on_disconnect(cmd)) {
        cJSON_AddTrueToObject(root, "rbOnDisconnect");
    }
    cJSON_AddStringToObject(root, OEJSON_EXTENTNAME, ConCmd_get_extentname(cmd));
    cJSON_AddStringToObject(root, OEJSON_USERNAME, ConCmd_get_username(cmd));
    cJSON_AddStringToObject(root, OEJSON_PASSWORD, ConCmd_get_pwd(cmd)); //idiot todo:
    cJSON_AddItemToObject(root, OEJSON_VERSION, ver=cJSON_CreateObject());
    cJSON_AddNumberToObject(ver, OEJSON_MAJ, ConCmd_get_version_maj(cmd));
    cJSON_AddNumberToObject(ver, OEJSON_MIN, ConCmd_get_version_min(cmd));
    cJSON_AddNumberToObject(ver, OEJSON_SEQ, ConCmd_get_version_upt(cmd));
    oe_id cid =  OekMsg_get_cid(msg);
    if (cid) {
        cJSON_AddNumberToObject(root, OEJSON_CID, cid);
    }

    *strp = _get_json_string(root, arena);
    cJSON_Delete(root);

    return 0;
}

static int _con_response_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    ConRes res = OekMsg_get_con_res(msg);
    assert(res);

    cJSON *root, *ver;
    root=cJSON_CreateObject();  
    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_RESPONSE);
    cJSON_AddStringToObject(root, OEJSON_NAME, OEJSON_CON);
    cJSON_AddNumberToObject(root, OEJSON_SID, ConRes_get_sid(res));
    cJSON_AddItemToObject(root, OEJSON_VERSION, ver=cJSON_CreateObject());
    cJSON_AddNumberToObject(ver, OEJSON_MAJ, ConRes_get_version_maj(res));
    cJSON_AddNumberToObject(ver, OEJSON_MIN, ConRes_get_version_min(res));
    cJSON_AddNumberToObject(ver, OEJSON_SEQ, ConRes_get_version_upt(res));
    oe_id cid =  OekMsg_get_cid(msg);
    if (cid) {
        cJSON_AddNumberToObject(root, OEJSON_CID, cid);
    }

    *strp = _get_json_string(root, arena);
    cJSON_Delete(root);

    return 0;
}

static int _put_command_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    PutCmd cmd = OekMsg_get_put_cmd(msg);
    DataObject data = PutCmd_get_data(cmd);

    cJSON *root, *values;
    root=cJSON_CreateObject();  
    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_COMMAND);
    cJSON_AddStringToObject(root, OEJSON_NAME, OEJSON_PUT);
    oe_id cid =  OekMsg_get_cid(msg);
    if (cid) {
        cJSON_AddNumberToObject(root, OEJSON_CID, cid);
    }
    oe_id tid =  OekMsg_get_tid(msg);
    if (tid) {
        cJSON_AddNumberToObject(root, OEJSON_TID, tid);
    }
    oe_dur dur =  PutCmd_get_dur(cmd);
    if (dur) {
        cJSON_AddNumberToObject(root, OEJSON_DUR, dur);
    }

    int nbytes =  DataObject_get_nbytes(data);
    if (nbytes) {
        size_t len = modp_b64_encode_len(nbytes);
        if (len <= 0) return -70;
        char *bytes = Arena_alloc(OekMsg_get_arena(msg), len, __FILE__, __LINE__);
        int rc = modp_b64_encode(bytes, DataObject_get_bytes(data), nbytes);
        if (rc <= 0) return -71;
        cJSON_AddStringToObject(root, OEJSON_BYTES, bytes);
    }

    if (PutCmd_is_tuple(cmd)) {
        values = cJSON_CreateArray();
        cJSON_AddItemToObject(root, OEJSON_VALUES, values);

        for ( Iterator iter = DataObject_iterator(data, true);
            Iterator_hasMore(iter);) {
            oe_scalar key = Iterator_next(iter);
            cJSON_AddItemToArray(values, cJSON_CreateString(key));
        }
    } else {
        values = cJSON_CreateObject();
        cJSON_AddItemToObject(root, OEJSON_VALUES, values);

        for ( Iterator iter = DataObject_iterator(data, true);
            Iterator_hasMore(iter);) {
            oe_scalar key = Iterator_next(iter);
            oe_scalar val = Iterator_next(iter);
            cJSON_AddItemToObject(values, key, cJSON_CreateString(val));
        }
    }

    *strp = _get_json_string(root, arena);
    cJSON_Delete(root);

    return 0;
}

static int _put_response_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    PutRes res = OekMsg_get_put_res(msg);
    assert(res);

    cJSON *root;
    root=cJSON_CreateObject();  
    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_RESPONSE);
    cJSON_AddStringToObject(root, OEJSON_NAME, OEJSON_PUT);
    //cJSON_AddNumberToObject(root, OEJSON_EXPTIME, PutRes_get_expiretime(res));
    cJSON_AddNumberToObject(root, OEJSON_LID, PutRes_get_lease_id(res));
    oe_id cid =  OekMsg_get_cid(msg);
    if (cid) {
        cJSON_AddNumberToObject(root, OEJSON_CID, cid);
    }

    *strp = _get_json_string(root, arena);
    cJSON_Delete(root);

    return 0;
}

static int _get_command_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    GetCmd cmd = OekMsg_get_get_cmd(msg);
    DataObjectList templates = GetCmd_get_data_objects(cmd);
    assert(templates);

    cJSON *root, *jtemplates;
    root=cJSON_CreateObject();  
    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_COMMAND);
    cJSON_AddStringToObject(root, OEJSON_NAME, OEJSON_GET);

    oe_id cid = OekMsg_get_cid(msg);
    if (cid) cJSON_AddNumberToObject(root, OEJSON_CID, cid);

    oe_id tid = OekMsg_get_tid(msg);
    if (tid) cJSON_AddNumberToObject(root, OEJSON_TID, tid);

    int timeout = GetCmd_get_timeout(cmd);
    if (timeout) cJSON_AddNumberToObject(root, OEJSON_TIMEOUT,  timeout);

    int maxresults = GetCmd_get_max_responses(cmd);
    if (maxresults) cJSON_AddNumberToObject(root, OEJSON_MAXRESULTS,  maxresults);

    if (GetCmd_remove(cmd)) {
        cJSON_AddItemToObject(root, OEJSON_REMOVE,  cJSON_CreateTrue());
    }

    if (GetCmd_ifexists(cmd)) {
        cJSON_AddItemToObject(root, OEJSON_IFEXISTS,  cJSON_CreateTrue());
    }

    if (GetCmd_return_attrs(cmd)) {
        cJSON_AddItemToObject(root, OEJSON_RETURN_ATTRS,  cJSON_CreateTrue());
    }

    if (GetCmd_return_bytes(cmd)) {
        cJSON_AddItemToObject(root, OEJSON_RETURN_BYTES,  cJSON_CreateTrue());
    }

    if (GetCmd_get_join(cmd)) {
        cJSON_AddItemToObject(root, OEJSON_ISJOIN,  cJSON_CreateTrue());
    }

    // get arraylist and add each array to the data obj list
    jtemplates = cJSON_CreateArray();
    cJSON_AddItemToObject(root, OEJSON_TEMPLATES, jtemplates);

    Iterator templates_iter = DataObjectList_iterator(templates, true);
    for (; Iterator_hasMore(templates_iter); ) {

        if (GetCmd_is_tuple(cmd)) {
            cJSON *jtemplate = cJSON_CreateArray();
            cJSON_AddItemToArray(jtemplates, jtemplate);
            DataObject templt = Iterator_next(templates_iter);
            int nkeys = DataObject_get_nattrs(templt);
            oe_scalar *keylist = DataObject_toArray(templt);
            for (int j = 0; j < nkeys; j++) {
                cJSON_AddItemToArray(jtemplate, cJSON_CreateString(keylist[j]));
            }
        } else {
            cJSON *jtemplate = cJSON_CreateObject();
            cJSON_AddItemToArray(jtemplates, jtemplate);
            DataObject templt = Iterator_next(templates_iter);
            for (Iterator di = DataObject_iterator(templt, true);
                Iterator_hasMore(di);) {
                oe_scalar key = Iterator_next(di);
                oe_scalar val = Iterator_next(di);
                cJSON_AddItemToObject(jtemplate, key, cJSON_CreateString(val));
            }
        }
    }

    *strp = _get_json_string(root, arena);
    cJSON_Delete(root);

    return 0;
}

static int _get_response_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    GetRes res = OekMsg_get_get_res(msg);
    DataObject data = GetRes_get_data(res);

    cJSON *root, *values;
    root=cJSON_CreateObject();  
    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_RESPONSE);
    cJSON_AddStringToObject(root, OEJSON_NAME, OEJSON_GET);

    oe_id cid =  OekMsg_get_cid(msg);
    if (cid) {
        cJSON_AddNumberToObject(root, OEJSON_CID, cid);
    }
    oe_id tid =  OekMsg_get_tid(msg);
    if (tid) {
        cJSON_AddNumberToObject(root, OEJSON_TID, tid);
    }
    if (GetRes_is_destroyed(res)) {
        cJSON_AddItemToObject(root, OEJSON_RETURN_IS_DESTROYED,  cJSON_CreateTrue());
    }

    cJSON_AddNumberToObject(root, OEJSON_NRESULTS, GetRes_get_nresults(res));

    if (GetRes_return_bytes(res)) {
        int nbytes =  DataObject_get_nbytes(data);
        if (nbytes) {
            size_t len = modp_b64_encode_len(nbytes);
            if (len <= 0) return -72;
            char *bytes = Arena_alloc(OekMsg_get_arena(msg), len, __FILE__, __LINE__);
            int rc = modp_b64_encode(bytes, DataObject_get_bytes(data), nbytes);
            if (rc <= 0) return -73;
            cJSON_AddStringToObject(root, OEJSON_BYTES, bytes);
        }
    }
    if (GetRes_return_attrs(res)) {
        int nkeys = DataObject_get_nattrs(data);
        if (nkeys) {
            if (GetRes_is_tuple(res)) {
                values = cJSON_CreateArray();
                cJSON_AddItemToObject(root, OEJSON_VALUES, values);

                for ( Iterator iter = DataObject_iterator(data, true);
                    Iterator_hasMore(iter);) {
                    oe_scalar key = Iterator_next(iter);
                    cJSON_AddItemToArray(values, cJSON_CreateString(key));
                }
            } else {
                values = cJSON_CreateObject();
                cJSON_AddItemToObject(root, OEJSON_VALUES, values);
                for (Iterator di = DataObject_iterator(data, true);
                    Iterator_hasMore(di);) {
                    oe_scalar key = Iterator_next(di);
                    oe_scalar val = Iterator_next(di);
                    cJSON_AddItemToObject(values, key, cJSON_CreateString(val));
                }
            }
        }
    }

    *strp = _get_json_string(root, arena);
    cJSON_Delete(root);

    return 0;
}

static int _sta_command_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    StaCmd cmd = OekMsg_get_sta_cmd(msg);

    cJSON *root;
    root=cJSON_CreateObject();  
    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_COMMAND);
    cJSON_AddStringToObject(root, OEJSON_NAME, OEJSON_STA);

    oe_id cid = OekMsg_get_cid(msg);
    if (cid) cJSON_AddNumberToObject(root, OEJSON_CID, cid);

    oe_dur dur = StaCmd_get_dur(cmd);
    if (dur) cJSON_AddNumberToObject(root, OEJSON_DUR,  dur);

    *strp = _get_json_string(root, arena);
    cJSON_Delete(root);

    return 0;
}

static int _sta_response_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    int rc = 0;

    StaRes res = OekMsg_get_sta_res(msg);

    cJSON *root;
    root=cJSON_CreateObject();  

    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_RESPONSE);
    cJSON_AddStringToObject(root, OEJSON_NAME, OEJSON_STA);

    oe_id cid = OekMsg_get_cid(msg);
    if (cid) cJSON_AddNumberToObject(root, OEJSON_CID, cid);

    oe_id tid = OekMsg_get_tid(msg);
    if (!tid) rc = -1;
    else cJSON_AddNumberToObject(root, OEJSON_TID, tid);

    //oe_time exptime = StaRes_get_expiretime(res);
    //if (exptime) cJSON_AddNumberToObject(root, OEJSON_EXPTIME,  exptime);

    *strp = _get_json_string(root, arena);

    cJSON_Delete(root);

    return rc;
}

static int _upl_command_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    UplCmd cmd = OekMsg_get_upl_cmd(msg);

    cJSON *root;
    root=cJSON_CreateObject();  
    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_COMMAND);
    cJSON_AddStringToObject(root, OEJSON_NAME, OEJSON_UPL);

    oe_id cid = OekMsg_get_cid(msg);
    if (cid) cJSON_AddNumberToObject(root, OEJSON_CID, cid);

    oe_dur dur = UplCmd_get_dur(cmd);
    if (dur) cJSON_AddNumberToObject(root, OEJSON_DUR,  dur);

    oe_id lid = UplCmd_get_lid(cmd);
    if (lid) cJSON_AddNumberToObject(root, OEJSON_LID,  lid);

    *strp = _get_json_string(root, arena);
    cJSON_Delete(root);

    return 0;
}

static int _upl_response_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    UplRes res = OekMsg_get_upl_res(msg);

    cJSON *root;
    root=cJSON_CreateObject();  
    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_RESPONSE);
    cJSON_AddStringToObject(root, OEJSON_NAME, OEJSON_UPL);

    oe_id cid = OekMsg_get_cid(msg);
    if (cid) cJSON_AddNumberToObject(root, OEJSON_CID, cid);

    oe_time exptime = UplRes_get_expiretime(res);
    if (exptime) cJSON_AddNumberToObject(root, OEJSON_EXPTIME,  exptime);

    *strp = _get_json_string(root, arena);
    cJSON_Delete(root);

    return 0;
}

static int _upt_command_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    UptCmd cmd = OekMsg_get_upt_cmd(msg);

    cJSON *root;
    root=cJSON_CreateObject();  
    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_COMMAND);
    cJSON_AddStringToObject(root, OEJSON_NAME, OEJSON_UPT);

    oe_id cid = OekMsg_get_cid(msg);
    if (cid) cJSON_AddNumberToObject(root, OEJSON_CID, cid);

    oe_id tid = OekMsg_get_tid(msg);
    if (tid) cJSON_AddNumberToObject(root, OEJSON_TID, tid);

    oe_dur dur = UptCmd_get_dur(cmd);
    if (dur) cJSON_AddNumberToObject(root, OEJSON_DUR,  dur);

    int enlisted = UptCmd_get_enlisted(cmd);
    cJSON_AddNumberToObject(root, OEJSON_ENLISTED,  enlisted);

    char status = UptCmd_get_status(cmd);
    char status_str[2];
    status_str[0] = status;
    status_str[1] = NULL;
    if (status) cJSON_AddStringToObject(root, OEJSON_STATUS, status_str);

    *strp = _get_json_string(root, arena);
    cJSON_Delete(root);

    return 0;
}

static int _upt_response_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    UptRes res = OekMsg_get_upt_res(msg);

    cJSON *root;
    root=cJSON_CreateObject();  
    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_RESPONSE);
    cJSON_AddStringToObject(root, OEJSON_NAME, OEJSON_UPT);

    oe_id cid = OekMsg_get_cid(msg);
    if (cid) cJSON_AddNumberToObject(root, OEJSON_CID, cid);

    /*
    oe_time exptime = UptRes_get_expiretime(res);
    if (exptime) cJSON_AddNumberToObject(root, OEJSON_EXPTIME,  exptime);
    */

    char status = UptRes_get_status(res);
    char status_str[2];
    status_str[0] = status;
    status_str[1] = NULL;
    if (status) cJSON_AddStringToObject(root, OEJSON_STATUS, status_str);

    *strp = _get_json_string(root, arena);
    cJSON_Delete(root);

    return 0;
}

static int _err_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    OekErr err = OekMsg_get_err(msg);

    cJSON *root;
    root=cJSON_CreateObject();  
    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_ERROR);

    oe_id cid = OekMsg_get_cid(msg);
    if (cid) cJSON_AddNumberToObject(root, OEJSON_CID, cid);

    oe_id etype = OekErr_get_type(err);
    if (etype) cJSON_AddNumberToObject(root, OEJSON_EID,  etype);

    char *emsg = OekErr_get_message(err);
    if (emsg) cJSON_AddStringToObject(root, OEJSON_MESSAGE,  emsg);

    *strp = _get_json_string(root, arena);
    cJSON_Delete(root);

    return 0;
}

static int _dis_command_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    DisCmd cmd = OekMsg_get_dis_cmd(msg);

    cJSON *root;
    root=cJSON_CreateObject();  
    cJSON_AddStringToObject(root, OEJSON_TYPE, OEJSON_COMMAND);
    cJSON_AddStringToObject(root, OEJSON_NAME, OEJSON_DIS);

    *strp = _get_json_string(root, arena);
    cJSON_Delete(root);

    return 0;
}

/*

//ERR corid:id type:id msg:string
static const char *err_msg = "ERR %llu %llu %s\n\n";
static int _error_serialize (const OekMsg msg, char **str, Arena_T arena) {

    OekErr err = OekMsg_get_err(msg);

    oe_chars message = OekErr_get_message(err);
    int bufflen = strlen(err_msg) + 2 * sizeof(oe_id) + strlen((char *) message) + 1;
    char *buff;
    buff = Arena_calloc(arena, 1, bufflen, __FILE__, __LINE__);

    snprintf(buff, bufflen, err_msg, OekMsg_get_cid(msg), OekErr_get_type(err), message );

    *str = buff;

    return 0;
}

*/

int _command_create(OekMsg *msgp, cJSON *root, Arena_T arena) {

    cJSON *namenode = cJSON_GetObjectItem(root,OEJSON_NAME);
    if (!namenode) return -74;
    if (!namenode->valuestring) return -75;
    char *name = namenode->valuestring;

    OekMsg msg = NULL;
    int rc = 0;
    if ( strcmp(name, OEJSON_CON) == 0) {
        msg = OekMsg_new(CON_CMD, arena);
        rc = _con_command_create(msg, root);
    } else if ( strcmp(name, OEJSON_PUT) == 0) {
        msg = OekMsg_new(PUT_CMD, arena);
        rc = _put_command_create(msg, root);
    } else if ( strcmp(name, OEJSON_GET) == 0) {
        msg = OekMsg_new(GET_CMD, arena);
        rc = _get_command_create(msg, root);
    } else if ( strcmp(name, OEJSON_STA) == 0) {
        msg = OekMsg_new(STA_CMD, arena);
        rc = _sta_command_create(msg, root);
    } else if ( strcmp(name, OEJSON_UPL) == 0) {
        msg = OekMsg_new(UPL_CMD, arena);
        rc = _upl_command_create(msg, root);
    } else if ( strcmp(name, OEJSON_UPT) == 0) {
        msg = OekMsg_new(UPT_CMD, arena);
        rc = _upt_command_create(msg, root);
    } else if ( strcmp(name, OEJSON_DIS) == 0) {
        msg = OekMsg_new(DIS_CMD, arena);
        rc = _dis_command_create(msg, root);
    } else {
        rc = -76;
    }
    if (!rc) *msgp = msg;
    return rc;
}

int _response_create(OekMsg *msgp, cJSON *root, Arena_T arena) {

    cJSON *node = cJSON_GetObjectItem(root,OEJSON_NAME);
    if (!node) return -77;
    if (!node->valuestring) return -78;
    char *name = node->valuestring;

    OekMsg msg = NULL;
    int rc = 0;
    if ( strcmp(name, OEJSON_CON) == 0) {
        msg = OekMsg_new(CON_RES, arena);
        rc = _con_response_create(msg, root);
    } else if ( strcmp(name, OEJSON_PUT) == 0) {
        msg = OekMsg_new(PUT_RES, arena);
        rc = _put_response_create(msg, root);
    } else if ( strcmp(name, OEJSON_GET) == 0) {
        msg = OekMsg_new(GET_RES, arena);
        rc = _get_response_create(msg, root);
    } else if ( strcmp(name, OEJSON_STA) == 0) {
        msg = OekMsg_new(STA_RES, arena);
        rc = _sta_response_create(msg, root);
    } else if ( strcmp(name, OEJSON_UPL) == 0) {
        msg = OekMsg_new(UPL_RES, arena);
        rc = _upl_response_create(msg, root);
    } else if ( strcmp(name, OEJSON_UPT) == 0) {
        msg = OekMsg_new(UPT_RES, arena);
        rc = _upt_response_create(msg, root);
    } else {
        rc = -79;
    }
    if (!rc) *msgp = msg;
    return rc;
}

int json_msg_serialize(const OekMsg msg, char **strp, Arena_T arena) {

    OEK_MSG_TYPE mtype = OekMsg_get_type(msg);

    switch (mtype) {
    case CON_CMD:
        return _con_command_serialize(msg, strp, arena);
    case CON_RES:
        return _con_response_serialize(msg, strp, arena);
    case PUT_CMD:
        return _put_command_serialize(msg, strp, arena);
    case PUT_RES:
        return _put_response_serialize(msg, strp, arena);
    case GET_CMD:
        return _get_command_serialize(msg, strp, arena);
    case GET_RES:
        return _get_response_serialize(msg, strp, arena);
    case STA_CMD:
        return _sta_command_serialize(msg, strp, arena);
    case STA_RES:
        return _sta_response_serialize(msg, strp, arena);
    case UPL_CMD:
        return _upl_command_serialize(msg, strp, arena);
    case UPL_RES:
        return _upl_response_serialize(msg, strp, arena);
    case UPT_CMD:
        return _upt_command_serialize(msg, strp, arena);
    case UPT_RES:
        return _upt_response_serialize(msg, strp, arena);
    case DIS_CMD:
        return _dis_command_serialize(msg, strp, arena);
    case OEK_ERR:
        return _err_serialize(msg, strp, arena);
    case OEK_NON:
    default:
        assert(false);
    }
}

int json_msg_create(OekMsg *msgp, char *str, Arena_T arena) {

    cJSON *root = cJSON_Parse(str);
    if (root == NULL) {
        return -84;
    }
    cJSON *typenode = cJSON_GetObjectItem(root,OEJSON_TYPE);
    if (!typenode) return -80;
    if (!typenode->valuestring) return -81;

    int rc = 0;

    if ( strcmp(typenode->valuestring, OEJSON_COMMAND) == 0) {
        rc = _command_create(msgp, root, arena);
    } else if ( strcmp(typenode->valuestring, OEJSON_RESPONSE) == 0) {
        rc = _response_create(msgp, root, arena);
    } else if ( strcmp(typenode->valuestring, OEJSON_ERROR) == 0) {
        rc = _error_create(msgp, root, arena);
    } else {
        rc = -82;
    }

    cJSON_Delete(root);
    return rc;
}

