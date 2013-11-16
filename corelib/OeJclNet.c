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
#include "OeJclDecoder.h"
#include "OeJclEncoder.h"
#include "oescript_core_resources.h"
#include "OeJclNet.h"
#include "OeNetConn.h"
#include "OeSession.h"

static char *MSG_END_BYTES = (char *) "\n\n";

int OeJclNet_read_msg(OeSession client, OekMsg *msgp, Arena_T arena) {

    OeNetConn conn = OeSession_get_conn(client);

    if (!OeNetConn_available(conn)) return 0;

    int ret = 0;

    int pos = OeNetConn_get_pos(conn, MSG_END_BYTES, 2);
    if (pos < 0) {
        OE_DLOG(0, OEJCL_PARTIAL_READ);
        return ret;
    }

    size_t input_sz = pos + 2;

    u_char *input;
    input = Arena_alloc(arena, input_sz + 1, __FILE__, __LINE__);
    input[input_sz] = 0;

    //size_t amt = 
    OeNetConn_read(conn, input, input_sz);
#ifdef OE_TRACE
    OE_DLOG(0, OEP_OEJCL_READMSG, input);
#endif

    OeJclDecoder decoder = OeJclDecoder_new(arena, input);
    if (decoder == NULL) {
        OE_ERR(0, OEJCL_BAD_PARSE, (char *) input);
        ret = -1;
        goto quit;
    }
    OekMsg msg = OeJclDecoder_next_cmd(decoder, NULL);
    if (msg == NULL) {
        OE_ERR(0, OEJCL_BAD_PARSE, (char *) input);
        ret = -1;
        goto quit;
    } else {
        *msgp = msg;
        ret = 1;
    }

    quit:
    return ret;
}

//static char CID_TAG[] = "cid\":";
static char CID_TAG[] = "cid=";

int OeJclNet_peek_cid(OeSession client, oe_id *cid) {

    OeNetConn conn = OeSession_get_conn(client);
    int avail = OeNetConn_available(conn);
    int pos = OeNetConn_get_pos(conn, CID_TAG, 4);
    if (pos < 0) {
        if ( OeSession_state_is_set( client, OENET_CLIENT_CONNECTED ) ) {
            return 0;
        }
        *cid = 1;
        return 1;//magic cor id :(
    }

    int lpos = pos + 4;
    int llen = lpos + 8;
    llen = llen < avail ? llen : avail - lpos - 1;
    void *peekdata = OeNetConn_peek(conn, lpos, llen);
    *cid = (oe_id) atol(peekdata);
    Mem_free(peekdata, __FILE__, __LINE__);
#ifdef OE_TRACE
    OE_DLOG(0, OEP_OEJCL_PEEK_CID, *cid);
#endif
    return 1;
}

int OeJclNet_write_msg(OeSession client, OekMsg msg, Arena_T arena) {

    assert(client);
    assert(msg);
    assert(arena);

    OeJclEncoder encoder = OeJclEncoder_new(arena);
    char *buff = OeJclEncoder_msg_serialize(encoder, msg);
#ifdef OE_TRACE
    OE_DLOG(0, OEP_OEJCL_WRITEMSG, buff);
#endif
    if (buff == NULL) return -1;
    int rc = OeNetConn_write(OeSession_get_conn(client), buff, strlen(buff));
    assert(!rc);
    rc = OeNetConn_write(OeSession_get_conn(client), "\n\n", 2);
    assert(!rc);
    return rc;
}

