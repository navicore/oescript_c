/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEP_RES_EN_US_H
#define OEP_RES_EN_US_H

#define OEP_STR_CON_TIMEOUT             "CON timeout closing socket client\n"

#define OEP_STR_CON_CLOSED              "connection to server closed"

#define OEP_NO_CID                      "ACContext not found for cid %lld\n"

#define OEP_BAD_SPEC                    "bad protocol spec: %s\n"

#define OEP_NO_CONN                     "conn not found. client did not get response. cid: %lld\n"

#define OEP_ERR_NOT_CONNECTED           "not connected"
#define OEP_ERR_NOT_LOGGED_IN           "not logged in"
#define OEP_ERR_CAN_NOT_PARSE           "can not parse msg"

#define OEP_DIS_CLOSE                   "DIS command processed"

#define OEP_CLIENT_RESTORE_C_W_CID      "OepClient_restore_context with cid: %i\n"
#define OEP_CLIENT_SAVE_C_W_CID         "OepClient_save_context with cid: %i\n"
#define OEP_CLIENT_RESTORE_C_SUCCESS    "OepClient_restore_context success. cid: %i\n"
#define OEP_CLIENT_READ_TYPE            "OepClient_buffered_on_read type: %i\n"
#define OEP_CLIENT_CB_NOT_SET           "*warning* OepClient__buffered_on_read write_responce_cb not configured for type: %i\n"
#define OEP_CLIENT_PARSE_ERROR          "can not parse response"
#define OEP_CLIENT_CONNECTED            "OepClient connected. sid: %lld\n"
#define OEP_CLIENT_CONNECT_ERR          "OepClient could not connect: %s\n"

#define OEP_SERVER_CONN_TIMEOUT          "connection timed out"
#define OEP_SERVER_ALREADY_CONNECTED     "already connected"
#define OEP_SERVER_ACCESS_DENIED         "access denied"

#endif

#ifdef __cplusplus
}
#endif 

