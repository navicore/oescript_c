/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "OeLibevent2Factory.h"
#include "OeLibeventConnFactory.h"
#include "OeNet.h"
#include "Oed_Dispatcher.h"

#include "config.h"
#include <assert.h>
#include "oec_values.h"
#include <event.h>
#include <event2/listener.h>
#include <event2/bufferevent_ssl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include "set.h"
#include "Oec_IdSet.h"
#include "OeNetConn.h"
#include "OeSession.h"
#include "OeNet.h"
#include "oescript_core_resources.h"

static void _buffered_close_on_writeable(struct bufferevent *bev, void *arg);
static void _buffered_on_error_cb(struct bufferevent *bev, short what, void *arg);

typedef struct ___net_holder {

    struct event_base *evbase;
    char *cert;   //file path for cert
    char *pkey;   //file path for pkey
    NET_TYPE ntype;
    SSL_CTX *server_ssl_ctx;
    SSL_CTX *client_ssl_ctx;

} OENET_HOLDER;

typedef struct ___port_info {

    int                 listen_fd;
    struct sockaddr_in  listen_addr;
    struct event        ev_accept;
    int                 reuseaddr_on;

} OENET_PORT_INFO;


static int _set_non_block(int fd) {

    int flags = fcntl(fd, F_GETFL);

    if (flags < 0) return flags;

    flags |= O_NONBLOCK;

    if (fcntl(fd, F_SETFL, flags) < 0) return -1;

    return 0;
}


static void _orderly_cleanup_ssl_conn(struct bufferevent *bev) {
    SSL *ctx = bufferevent_openssl_get_ssl(bev);
    if (ctx) {
        SSL_set_shutdown(ctx, SSL_SENT_SHUTDOWN | SSL_RECEIVED_SHUTDOWN);
        SSL_shutdown(ctx);
    }
}


static void _orderly_cleanup_conn(void *sessionp) {
    OeSession session = (OeSession) sessionp;
    OE_TLOG(0, "OeLibevent2Factory _orderly_cleanup_conn\n");
    OeNetConn conn = OeSession_get_conn(session);
    OeSession_state_set(session, OENET_CLIENT_CLOSING);
    struct bufferevent *bev = oenetconn_get_conn_obj(conn);
    _orderly_cleanup_ssl_conn(bev);
    bufferevent_setcb(bev, bev->readcb, _buffered_close_on_writeable, _buffered_on_error_cb, session);
    bufferevent_enable(bev, EV_WRITE);
}


static void _cleanup_conn(OeSession session) {

    OE_TLOG(0, "OeLibevent2Factory _cleanup_conn\n");

    assert(session);

    OeSession_state_clear(session, OENET_CLIENT_CONNECTED);
    OeSession_state_set(session, OENET_CLIENT_CLOSING);

    OeNetConn conn = OeSession_get_conn(session);
    assert(conn);
    if (oenetconn_get_timeout(conn)) {
        OE_DLOG(NULL, OENET_CLEANUP_PENDING_TIMOUT);
        void *timeout = oenetconn_get_timeout(conn);
        if (timeout) {
            timeout_del(timeout);
            OeNetConn_clear_timeout(conn);
        }
    }

    if (OeSession_get_task_count(session)) {
        OE_DLOG(0, OENET_STR_NOT_CLOSING_SOCK_CLIENT);
        //a pending task must complete, the state is closing so cleanup
        //  should get called again
        return;
    }

    OeNet _this_ = OeSession_get_net(session);
    Oec_IdSet idset = OeNet_get_sessions(_this_);
    Oec_IdSet_remove(idset, OeSession_get_id(session));
    struct bufferevent *bev = oenetconn_get_conn_obj(conn);
    if (bev->readcb) {
        bev->readcb(NULL, session);
    } else {
        OE_ERR(NULL, OENET_CLEANUP_NO_READ_CB);
    }

    OE_DLOG(NULL, OENET_STR_CLOSED_SOCK_CLIENT);
    OeSession_free(&session);
}

/**
 * Called by libevent when the write buffer reaches 0. Low 
 * watermark.  we only listen for these when we want to close 
 * the connection. 
 */
static void _buffered_close_on_writeable(struct bufferevent *bev, void *arg) {
    OE_TLOG(0, "OeLibevent2Factory _buffered_close_on_writeable\n");

    OE_DLOG(NULL, OENET_STR_BUFF_WRITE_CLOSE);
    OeSession session = (OeSession)arg;
    assert(session);
    _cleanup_conn(session);
}

static void _con_timeout_cb(int huh, short what, void *arg) {
    OE_TLOG(0, "OeLibevent2Factory _con_timeout_cb\n");

    OeSession session = (OeSession)arg;
    assert(session);
    OeNet _this_ = OeSession_get_net(session);
    assert(_this_);
    oenet_callback *fun = OeNet_get_con_timeout_cb(_this_);
    fun(session);
}

static void _read_handler_cb(struct bufferevent *bev, void *arg) {
    OE_TLOG(0, "OeLibevent2Factory _read_handler_cb\n");
    OeSession session = (OeSession)arg;
    if (!bev) OeSession_state_set(session, OENET_CLIENT_CLOSING);
    assert(session);
    OeNet _this_ = OeSession_get_net(session);
    assert(_this_);
    oenet_callback *fun = OeNet_get_read_handler(_this_);
    fun(session);
}

static void _buffered_on_error_cb(struct bufferevent *bev, short what, void *arg) {

    OeSession session = (OeSession)arg;
    OeNet _this_ = OeSession_get_net(session);

    if (what & BEV_EVENT_CONNECTED) {
        //todo: is this event a bogus fyi or is something not right with
        // the ssl?
        OE_DLOG(0, "BEV_EVENT_CONNECTED %s\n", OeNet_get_name(_this_));
        return; //noop
    }

    if (what & BEV_EVENT_ERROR) {
        unsigned long err;
        while ((err = (bufferevent_get_openssl_error(bev)))) {
            const char *msg = (const char*)
                              ERR_reason_error_string(err);
            const char *lib = (const char*)
                              ERR_lib_error_string(err);
            const char *func = (const char*)
                               ERR_func_error_string(err);
            OE_DLOG(0, "BEV_EVENT_ERROR %s: %s in %s %s\n", OeNet_get_name(_this_), msg, lib, func);
        }
        if (errno)
            perror("connection error");
    }

    if (what & BEV_EVENT_READING)   OE_DLOG(0, "BEV_EVENT_READING\n");
    if (what & BEV_EVENT_WRITING)   OE_DLOG(0, "BEV_EVENT_WRITING\n");
    if (what & BEV_EVENT_TIMEOUT)   OE_DLOG(0, "BEV_EVENT_TIMEOUT\n");

    OE_DLOG(NULL, OENET_STR_BUFF_ERR_CLOSE);
    assert(session);
    _cleanup_conn(session);
}

static void _accept_error_cb(struct evconnlistener *listener, void *ctx) {
    OE_TLOG(0, "OeLibevent2Factory _accept_error_cb\n");

    struct event_base *base = evconnlistener_get_base(listener);
    int err = EVUTIL_SOCKET_ERROR();
    OE_ERR(0, "Got an error %d (%s) on the listener. "
           "Shutting down.\n", err, evutil_socket_error_to_string(err)); //i18n
}

static void _watch_for_timeout( OeNet _this_, OeSession session ) {
    OE_TLOG(0, "OeLibevent2Factory _watch_for_timeout\n");

    oenet_callback *fun = OeNet_get_con_timeout_cb(_this_);
    if (!fun) return;

    struct timeval tv;
    struct event *timeout;

    timeout = Mem_calloc( 1, sizeof *timeout, __FILE__, __LINE__);
    OENET_HOLDER *holder = (OENET_HOLDER *) OeNet_get_net_obj(_this_);
    event_base_set(holder->evbase, timeout);
    oenetconn_set_timeout(OeSession_get_conn(session), timeout );

    evtimer_set(timeout, _con_timeout_cb, session);

    evutil_timerclear( &tv );
    tv.tv_sec = 10;
    event_add( timeout, &tv );
}

static void _accept_conn_cb(struct evconnlistener *listener,
                            evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx) {
    OE_TLOG(0, "OeLibevent2Factory _accept_conn_cb\n");
    _set_non_block(fd);

    OeNet _this_ = (OeNet) ctx;
    OENET_HOLDER *holder = (OENET_HOLDER *) OeNet_get_net_obj(_this_);

    struct event_base *base = evconnlistener_get_base(listener);

    struct bufferevent *bev;

    if (holder->ntype == OE_TCP_SSL) {
        SSL *client_ctx = SSL_new(holder->server_ssl_ctx);
        bev = bufferevent_openssl_socket_new(base, fd, client_ctx,
                                             BUFFEREVENT_SSL_ACCEPTING,
                                             BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
    } else {
        bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
    }

    OeNet_listen_for_bytes(_this_, bev);
}

//setup socket reader once both ends are connected
//static OeSession _listen_for_bytes( OeNet _this_, void *no ) {
static void *_listen_for_bytes( OeNet _this_, void *no ) {
//typedef void *   OENET_listen_for_bytes     (struct net_T*, void *);

    OE_TLOG(0, "OeLibevent2Factory _listen_for_bytes on %s\n", OeNet_get_name(_this_));

    assert(_this_); assert(no);

    OeSession session;
    session = oesession_new(_this_);
    OeSession_set_id(session, OeNet_create_id(_this_));
    Oec_IdSet idset = OeNet_get_sessions(_this_);
    assert(idset);
    Oec_IdSet_put(idset, OeSession_get_id(session));

    struct bufferevent *buf_ev = (struct bufferevent *) no;
    _watch_for_timeout(_this_, session);

    oenetconn_set_conn_obj(OeSession_get_conn(session), buf_ev);

    bufferevent_setcb(buf_ev, 
                      _read_handler_cb, 
                      NULL,
                      _buffered_on_error_cb, 
                      session);
    bufferevent_enable(buf_ev, EV_READ);

    OE_DLOG(NULL,OENET_STR_LISTEN_NEW_CLIENT);

    return (void *) session;
}

static OeNetConn _new_conn(OeNet _this_) {
    return OeLibeventConnFactory_create();
}


static void _evssl_init_print_err() {

    //i18n
    puts("Fatal Error: Couldn't read 'pkey' or 'cert' file.\n"
         "To generate a key and self-signed certificate, run:\n"
         "    openssl genrsa -out pkey 2048\n"
         "    openssl req -new -key pkey -out cert.req\n"
         "    openssl x509 -req -days 365 -in cert.req -signkey pkey -out cert");
}

static int _evssl_client_init(OENET_HOLDER *holder) {

    OE_TLOG(0, "OeLibevent2Factory client _evssl_init\n");
    SSL_CTX  *client_ctx;

    /* Initialize the OpenSSL library */
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_ciphers();
    /* We MUST have entropy, or else there's no point to crypto. */
    if (!RAND_poll())
        return -1;

    client_ctx = SSL_CTX_new(SSLv23_client_method());

    if (holder->cert && !SSL_CTX_use_certificate_file(client_ctx, holder->cert, SSL_FILETYPE_PEM)) {
        _evssl_init_print_err();
        return -1;
    }
    if (holder->pkey && !SSL_CTX_use_PrivateKey_file(client_ctx, holder->pkey, SSL_FILETYPE_PEM)) {
        _evssl_init_print_err();
        return -1;
    }
    SSL_CTX_set_options(client_ctx, SSL_OP_NO_SSLv2);

    holder->client_ssl_ctx = client_ctx;
    OE_ILOG(0, "client ssl is initialized\n");//i18n
    return 0;
}

static int _evssl_server_init(OENET_HOLDER *holder) {

    OE_TLOG(0, "OeLibevent2Factory _evssl_init\n");
    SSL_CTX  *server_ctx;

    /* Initialize the OpenSSL library */
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_ciphers();
    /* We MUST have entropy, or else there's no point to crypto. */
    if (!RAND_poll())
        return -1;

    server_ctx = SSL_CTX_new(SSLv23_server_method());

    if (holder->cert && !SSL_CTX_use_certificate_chain_file(server_ctx, holder->cert)) {
        _evssl_init_print_err();
        return -1;
    }
    if (holder->pkey && !SSL_CTX_use_PrivateKey_file(server_ctx, holder->pkey, SSL_FILETYPE_PEM)) {
        _evssl_init_print_err();
        return -1;
    }
    SSL_CTX_set_options(server_ctx, SSL_OP_NO_SSLv2);

    holder->server_ssl_ctx = server_ctx;
    OE_ILOG(0, "ssl is initialized\n");//i18n
    return 0;
}

//returns bev
static void * _connect(OeNet _this_, const char *hostname, int port) {

    OE_TLOG(0, "OeLibevent2Factory _connect %s\n", OeNet_get_name(_this_));

    struct hostent *hp;
    hp = gethostbyname(hostname);
    if (!hp) {
        OE_ERR(0, "OeLibevent2Factory no hostent\n");//i18n
        return NULL;
    }

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons( port );
    servaddr.sin_addr.s_addr = ((struct in_addr *) (hp->h_addr))->s_addr;

    int sock = socket( AF_INET, SOCK_STREAM, 0);
    int rc = connect( sock, (struct sockaddr *) &servaddr, (socklen_t) sizeof(struct sockaddr_in) );
    if (rc != 0) {
        OE_ERR(0, "OeLibevent2Factory _connect can not connect\n");//i18n
        return NULL;
    }
    _set_non_block(sock);

    OENET_HOLDER *holder = (OENET_HOLDER *) OeNet_get_net_obj(_this_);
    struct event_base *base = holder->evbase;
    if (holder->ntype == OE_TCP_SSL && !holder->client_ssl_ctx) {
        int rc = _evssl_client_init(holder);
        if (rc) {
            OE_ERR(0, "client ssl init failed\n"); //i18n
            return NULL;
        }
    }

    struct bufferevent *bev;
    if (holder->ntype == OE_TCP_SSL) {
        SSL *client_ctx = SSL_new(holder->client_ssl_ctx);
        assert(client_ctx);
        bev = bufferevent_openssl_socket_new(base, sock, client_ctx,
                                             BUFFEREVENT_SSL_CONNECTING,
                                             BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
        OE_DLOG(0, "client cypher: %s\n", SSL_get_cipher_name(client_ctx)); //i18n
    } else {
        bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
    }

    assert(bev);

    return bev;
}

//setup socket server
static void _listen_for_connect(OeNet _this_, char *hostifc, int port) {
    OE_TLOG(0, "OeLibevent2Factory _listen_for_connect\n");

    assert(_this_);
    assert(port);
    OENET_HOLDER *holder = (OENET_HOLDER *) OeNet_get_net_obj(_this_);
    struct event_base *evbase = holder->evbase;
    assert(evbase);
    if (holder->ntype == OE_TCP_SSL && !holder->server_ssl_ctx) {
        int rc = _evssl_server_init(holder);
        if (rc) {
            OE_ERR(0, "ssl init failed\n");
            return;
        }
    }

    OENET_PORT_INFO *pinfo;
    pinfo = Mem_calloc(1, sizeof *pinfo, __FILE__, __LINE__); //todo: fix leak incase you 
                                                              //wanna start and stop servers 
                                                              //more than once per invoke
    pinfo->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(pinfo->listen_fd >= 0);

    memset(&pinfo->listen_addr, 0, sizeof(pinfo->listen_addr));
    pinfo->listen_addr.sin_family      = AF_INET;
    pinfo->listen_addr.sin_addr.s_addr = INADDR_ANY;
    pinfo->listen_addr.sin_port        = htons(port);

    pinfo->reuseaddr_on = 1;

    struct evconnlistener *listener;
    listener = evconnlistener_new_bind(evbase, _accept_conn_cb, _this_,
                                       LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
                                       (struct sockaddr*)&pinfo->listen_addr, 
                                       sizeof(pinfo->listen_addr));
    if (!listener) {
        OE_ERR(NULL,"OeLibevent2Factory: Could not create listener: errno: %i %s\n", 
               errno, strerror(errno)); //i18n
        return;
    }
    _set_non_block(pinfo->listen_fd);
    evconnlistener_set_error_cb(listener, _accept_error_cb);
    OE_ILOG(NULL,OENET_STR_SETUP, port);
}

OeNet OeLibevent2Factory_create(Oed_Dispatcher dispatcher,
                                const char *cert, 
                                const char* pkey, 
                                NET_TYPE ntype) {
    OE_TLOG(0, "OeLibevent2Factory_create\n");
    OENET_HOLDER *holder;
    holder = Mem_calloc(1, sizeof *holder, __FILE__, __LINE__); //todo: fix leak incase you 
    //wanna start and stop servers 
    //more than once per invoke
    if (dispatcher) {
        holder->evbase =  (struct event_base *) Oed_Dispatcher_get_event_base(dispatcher);
    } else {
        holder->evbase = NULL;
    }
    holder->cert = oec_cpy_str(NULL, (oe_scalar) cert);
    holder->pkey =  oec_cpy_str(NULL, (oe_scalar) pkey);
    holder->ntype = ntype;

    OeNet net = OeNet_new(NULL, holder, 
                          _listen_for_connect, 
                          _orderly_cleanup_conn, 
                          _connect, 
                          (OENET_listen_for_bytes *) _listen_for_bytes, 
                          _new_conn);
    assert(net);
    return net;
}

/*
extern T OeNet_new(void                       *proto,
                   void                       *net_obj,
                   OENET_listen_for_connect   *listen,
                   OENET_cleanup_conn         *orderly_cleanup_conn_fun,
                   OENET_connect              *connect_fun,
                   OENET_listen_for_bytes     *listen_for_bytes_fun,
                   OENET_new_conn             *new_conn_fun);
*/

