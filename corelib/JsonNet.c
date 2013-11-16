/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include <string.h>
#include "oe_common.h"
#include "OEK.h"
#include "OekMsg.h"
#include "Oed_Dispatcher.h"
#include "json_msg_factory.h"
#include "oescript_core_resources.h"
#include "JsonNet.h"
#include "arena.h"
#include "OeNetConn.h"
#include "OeSession.h"

static char *MSG_END_BYTES = (char *) "\n\n";

int JsonNet_read_msg(OeSession client, OekMsg *msgp, Arena_T arena) {

    OeNetConn conn = OeSession_get_conn(client);

    if (!OeNetConn_available(conn)) return 0;

    int ret = 0;

    int pos = OeNetConn_get_pos(conn, MSG_END_BYTES, 2);
    if (pos < 0) {
        OE_DLOG(0, OEJSON_PARTIAL_READ);
        return ret;
    }

    size_t input_sz = pos + 2;

    u_char *input;
    input = Arena_alloc(arena, input_sz + 1, __FILE__, __LINE__);
    input[input_sz] = 0;

    //size_t amt = 
    OeNetConn_read(conn, input, input_sz);
#ifdef OE_TRACE
    OE_DLOG(0, OEP_JSON_READMSG, input);
#endif

    OekMsg msg;
    int rc = json_msg_create(&msg, input, arena);
    if (rc) {
        OE_ERR(0, OEJSON_BAD_PARSE, rc, (char *) input);
        ret = -1;
        goto quit;
    } else {
        *msgp = msg;
        ret = 1;
    }

    quit:
    return ret;
}

//static char CID_TAG[] = "\"cid\":";
static char CID_TAG[] = "cid\":";
//static char CID_TAG[] = "cid";

int JsonNet_peek_cid(OeSession client, oe_id *cid) {

    OeNetConn conn = OeSession_get_conn(client);
    int avail = OeNetConn_available(conn);
    int pos = OeNetConn_get_pos(conn, CID_TAG, 5);
    if (pos < 0) {
        if ( OeSession_state_is_set( client, OENET_CLIENT_CONNECTED ) ) {
            return 0;
        }
        *cid = 1;
        return 1;//magic cor id :(
    }

    int lpos = pos + 6;
    int llen = lpos + 8;
    llen = llen < avail ? llen : avail - lpos - 1;
    void *peekdata = OeNetConn_peek(conn, lpos, llen);
    *cid = (oe_id) atol(peekdata);
    Mem_free(peekdata, __FILE__, __LINE__);
#ifdef OE_TRACE
    OE_DLOG(0, OEP_JSON_PEEK_CID, *cid);
#endif
    return 1;
}

int JsonNet_write_msg(OeSession client, OekMsg msg, Arena_T arena) {

    assert(client);
    assert(msg);
    assert(arena);

    char *buff;
    int rc = json_msg_serialize(msg, &buff, arena);
    assert(!rc);
#ifdef OE_TRACE
    OE_DLOG(0, OEP_JSON_WRITEMSG, buff);
#endif
    if (rc) return rc;
    assert(buff);
    buff = oec_str_concat(arena, 2, buff, "\n\n"); //REMOVE!!! double copy todo
    rc = OeNetConn_write(OeSession_get_conn(client), buff, strlen(buff));
    assert(!rc);
    //rc = OeNetConn_write(OeSession_get_conn(client), "\n\n", 2); //todo: ejs get rid, the client error processing may have run dummy
    //assert(!rc);

    //worries.... it seams client/session can get cleaned up on this write.  either don't decr the
    // task count until done (the right thing) or test more and make sure you don't touch
    // the client after this call
    return rc;
}

