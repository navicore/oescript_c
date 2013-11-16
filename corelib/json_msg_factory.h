/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef JSON_MSG_F_H
#define JSON_MSG_F_H

#include <OekMsg.h>
#include <arena.h>

#define OEJSON_CID "cid"
#define OEJSON_TID "tid"
#define OEJSON_TYPE "t"
#define OEJSON_NAME "n"
#define OEJSON_COMMAND "cmd"
#define OEJSON_RESPONSE "res"
#define OEJSON_ERROR "err"
#define OEJSON_MESSAGE "msg"

#define OEJSON_DIS "disconnect"
#define OEJSON_CON "connect"
#define OEJSON_PUT "put"
#define OEJSON_GET "get"
#define OEJSON_UPT "utxn"
#define OEJSON_UPL "ulease"
#define OEJSON_STA "start"

#define OEJSON_SID "sid"
#define OEJSON_EXTENTNAME "extentname"
#define OEJSON_USERNAME "username"
#define OEJSON_PASSWORD "password"

#define OEJSON_VERSION "version"
#define OEJSON_MAJ "maj"
#define OEJSON_MIN "min"
#define OEJSON_SEQ "seq"

#define OEJSON_DUR "dur"
#define OEJSON_BYTES "bytes"
#define OEJSON_VALUES "values"
#define OEJSON_TEMPLATES "templates"
#define OEJSON_TIMEOUT "timeout"
#define OEJSON_MAXRESULTS "c"
#define OEJSON_ISJOIN "join"
#define OEJSON_IFEXISTS "ife"
#define OEJSON_REMOVE "rm"
#define OEJSON_RETURN_ATTRS "rattrs"
#define OEJSON_RETURN_BYTES "rbytes"
#define OEJSON_RETURN_IS_DESTROYED "rm"
#define OEJSON_LID "lid"
#define OEJSON_STATUS "s"
#define OEJSON_ENLISTED "c"
#define OEJSON_EID "eid"
#define OEJSON_NRESULTS "c"
#define OEJSON_EXPTIME "exp"

extern int json_msg_serialize(const OekMsg, char **, Arena_T);

extern int json_msg_create(OekMsg *, char *, Arena_T);

#endif

#ifdef __cplusplus
}
#endif 

