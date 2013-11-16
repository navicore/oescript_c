/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OEKMSG_H
#define OEKMSG_H

#include <string.h>
#include <stdbool.h>
#include "oec_values.h"
#include "arena.h"
#include "DataObject.h"

#include "OekErr.h"
#include "ConCmd.h"
#include "DisCmd.h"
#include "ConRes.h"
#include "PutCmd.h"
#include "PutRes.h"
#include "GetCmd.h"
#include "GetRes.h"
#include "StaCmd.h"
#include "StaRes.h"
#include "UptCmd.h"
#include "UptRes.h"
#include "UplCmd.h"
#include "UplRes.h"


enum _OEK_MSG_TYPE {
    OEK_NON,
    OEK_ERR,
    CON_CMD,
    DIS_CMD,
    CON_RES,
    STA_CMD,
    STA_RES,
    UPT_CMD,
    UPT_RES,
    UPL_CMD,
    UPL_RES,
    PUT_CMD,
    PUT_RES,
    GET_CMD,
    GET_RES,
};
typedef enum _OEK_MSG_TYPE OEK_MSG_TYPE;

#define T OekMsg
struct oekmsg_T {

    Arena_T arena;
    OEK_MSG_TYPE mtype;

    oe_id cor_id;
    oe_id txn_id;

    bool valid;

    union {
        OekErr err;
        DisCmd dis_cmd;
        ConCmd con_cmd;
        ConRes con_res;
        StaCmd sta_cmd;
        StaRes sta_res;
        UptCmd upt_cmd;
        UptRes upt_res;
        UplCmd upl_cmd;
        UplRes upl_res;
        PutCmd put_cmd;
        PutRes put_res;
        GetCmd get_cmd;
        GetRes get_res;
    } data;
};
typedef struct oekmsg_T *T;

/*
    a client/server protocol of commands and responses.

    the server is the implementation of the extent/shared-space.
    the client is the user application invoking the puts
    and gets of the coordination language enabled by the
    server api.

    commands are always issued by the client to the server.

    responses are always issues from the server to the client.

    there are no unsolicited messages to the client, though the
    responses may be delivered asynchronously.

    the server may override duration parameters and send the
    adjusted expiration time value in the reponse.

    correlation ids are chosen by the clients.  any integer
    value is allowed.  the scope of a cor_id is a singe
    command / response pair.  duplicates are allowed.
    response cor_id values echo the cor_id from the request.

    all errors and failures are reported by the server to the
    client in the error response message.  ie: if a put fails,
    a client will receive an error response instead of a put
    response.  the client must use the cor_id to correlate
    responses.

    username is the domain of transactions.  if you start a
    transaction with "rollback_on_disconnect = false" and get
    interrupted or disconnect as the natural order of your
    processing (maybe this is a long running txn that spans
    weeks!) and you want to add to the transaction or roll it
    back or commit it, then you must be connected with the
    same username as the start command used.
*/

extern T OekMsg_new( OEK_MSG_TYPE, Arena_T );
extern void OekMsg_settype(T, OEK_MSG_TYPE);

OEK_MSG_TYPE OekMsg_get_type(T);

extern void    OekMsg_set_cid(T, oe_id );
extern oe_id   OekMsg_get_cid(T);

extern void    OekMsg_set_tid(T, oe_id );
extern oe_id   OekMsg_get_tid(T);

extern OekErr  OekMsg_get_err(T);

extern ConCmd  OekMsg_get_con_cmd(T);
extern ConRes  OekMsg_get_con_res(T);

extern DisCmd  OekMsg_get_dis_cmd(T);

extern StaCmd  OekMsg_get_sta_cmd(T);
extern StaRes  OekMsg_get_sta_res(T);

extern UptCmd  OekMsg_get_upt_cmd(T);
extern UptRes  OekMsg_get_upt_res(T);

extern UplCmd  OekMsg_get_upl_cmd(T);
extern UplRes  OekMsg_get_upl_res(T);

extern GetCmd  OekMsg_get_get_cmd(T);
extern GetRes  OekMsg_get_get_res(T);

extern PutCmd  OekMsg_get_put_cmd(T);
extern PutRes  OekMsg_get_put_res(T);

extern Arena_T OekMsg_get_arena(T _this_);

#undef T
#endif

#ifdef __cplusplus
}
#endif 

