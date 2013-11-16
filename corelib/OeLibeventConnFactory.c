/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <stddef.h>
#include "oec_values.h"
#include "mem.h"
#include "OeNetConn.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <event.h>
#include <event2/bufferevent_ssl.h>
#include "OeLibeventConnFactory.h"

static void *_peek(OeNetConn conn, size_t pos, size_t len);

static void _free_conn_obj(void *conn_obj) {

    struct bufferevent *bev = (struct bufferevent *) conn_obj;

    /*
    SSL *ctx = bufferevent_openssl_get_ssl(bev);
    if (ctx) {
        SSL_set_shutdown(ctx, SSL_SENT_SHUTDOWN | SSL_RECEIVED_SHUTDOWN);
        SSL_shutdown(ctx);
    }
    */
    bufferevent_disable(bev, EV_READ);
    bufferevent_disable(bev, EV_WRITE);
    bufferevent_flush(bev, EV_WRITE, BEV_FINISHED);

    bufferevent_free(bev);
}

static int _get_pos(OeNetConn conn, char *needle, size_t needle_len) {

    assert(conn);
    struct bufferevent *bev = (struct bufferevent *) oenetconn_get_conn_obj(conn);
    struct evbuffer *eb = bev->input;

    size_t blen = evbuffer_get_length(eb);

    unsigned char *tmp = (unsigned char *) _peek(conn, 0, 0);
    //unsigned char tmp[blen + 1];
    //evbuffer_copyout(eb, tmp, blen);
    unsigned char *pos = (unsigned char *) oe_memmem(tmp, blen, needle, needle_len);
    if (!pos) {
        return -1;
    }
    int ret = pos - tmp;
    Mem_free(tmp, __FILE__, __LINE__);
    return ret;
}

static size_t _available(OeNetConn conn) {
    assert(conn);
    struct bufferevent *bev = (struct bufferevent *) oenetconn_get_conn_obj(conn);
    return evbuffer_get_length(bev->input);
}

static size_t _read(OeNetConn conn, void *data, size_t size) {
    assert(conn);
    struct bufferevent *bev = (struct bufferevent *) oenetconn_get_conn_obj(conn);
    return bufferevent_read(bev, data, size);
}

static int _write(OeNetConn conn, char *buffer, size_t size) {
    assert(conn);
    return bufferevent_write(oenetconn_get_conn_obj(conn), buffer, size);
}

static void *_peek(OeNetConn conn, size_t pos, size_t len) {
    assert(conn);
    struct bufferevent *bev = (struct bufferevent *) oenetconn_get_conn_obj(conn);
    struct evbuffer *eb = bev->input;
    if (len <= 0) { //grap all
        len = evbuffer_get_length(eb);
    }
    char *peekdata = Mem_calloc(1, len + 1, __FILE__, __LINE__);

    int tsz = pos + len;
    char tmp[tsz];
    evbuffer_copyout(eb, tmp, tsz);
    memcpy(peekdata, tmp+pos, len);

    return peekdata;
}

static void _clear_timeout(void *timeout) {
    free(timeout);
}

OeNetConn OeLibeventConnFactory_create() {
    return OeNetConn_new(_free_conn_obj, _get_pos, _available, _read, _write, _peek, _clear_timeout);
}

