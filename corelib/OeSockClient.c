/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <OeSockClient.h>
#include "oe_common.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#define T OeSockClient

struct T {
    int sock;
    struct sockaddr_in *servaddr;
    char *hostname;
    int port;
    SSL *sslHandle;
    SSL_CTX *sslContext;
    bool use_ssl;
};

static T _new( char *hostname, int port, bool use_ssl ) {
    T _this_;

    _this_ = Mem_calloc( 1, sizeof *_this_, __FILE__, __LINE__ );

    _this_->port = port;

    int hlen = strlen(hostname);
    _this_->hostname = Mem_alloc(hlen + 1, __FILE__, __LINE__ );
    strncpy(_this_->hostname, hostname, hlen + 1);
    _this_->servaddr = Mem_calloc(1, sizeof(struct sockaddr_in), __FILE__, __LINE__ );
    _this_->use_ssl = use_ssl;
    return _this_;
}

T OeSockClient_new( char *hostname, int port ) {
    return _new(hostname, port, false);
}

T OeSockClient_new_ssl( char *hostname, int port ) {
    return _new(hostname, port, true);
}

void OeSockClient_free( T *cp ) {
    assert(cp && *cp);
    T _this_ = *cp;
    OeSockClient_disconnect(_this_);
    Mem_free( _this_->servaddr, __FILE__, __LINE__ );
    Mem_free( _this_->hostname, __FILE__, __LINE__ );
    if (_this_->sslHandle)  SSL_free(_this_->sslHandle);
    if (_this_->sslContext) SSL_CTX_free(_this_->sslContext);
    Mem_free( _this_, __FILE__, __LINE__ );
}

int OeSockClient_connect( T _this_ ) {

    struct hostent *hp;
    hp = gethostbyname(_this_->hostname);
    if (!hp) return -1;

    _this_->servaddr->sin_family = AF_INET;
    _this_->servaddr->sin_port = htons( _this_->port );
    _this_->servaddr->sin_addr.s_addr = ((struct in_addr *) (hp->h_addr))->s_addr;

    int sock = socket( AF_INET, SOCK_STREAM, 0);
    int rc = connect( sock,
                      (struct sockaddr *)_this_->servaddr, (socklen_t) sizeof(struct sockaddr_in) );
    if (rc == 0)
        _this_->sock = sock;

    if (_this_->use_ssl && sock) {
        SSL_load_error_strings ();
        SSL_library_init ();

        OpenSSL_add_all_ciphers();

        SSL_CTX *sslContext = SSL_CTX_new (SSLv23_client_method ());
        if (sslContext == NULL) {
            return -1;
            ERR_print_errors_fp (stderr);
        }
        _this_->sslContext = sslContext;

        SSL_CTX_set_verify(sslContext, SSL_VERIFY_NONE, NULL);
        SSL_CTX_set_options(sslContext, SSL_OP_NO_SSLv2);

        SSL *sslHandle = SSL_new (sslContext);
        if (sslHandle == NULL) {
            ERR_print_errors_fp (stderr);
            return -1;
        }
        _this_->sslHandle = sslHandle;

        if (!SSL_set_fd (sslHandle, sock)) {
            ERR_print_errors_fp (stderr);
            return -1;
        }

        if (SSL_connect (sslHandle) != 1) {
            ERR_print_errors_fp (stderr);
            return -1;
        }

        SSL_set_mode(sslHandle, SSL_MODE_AUTO_RETRY);
    }

    return rc;
}

int OeSockClient_disconnect( T _this_ ) {
    return shutdown(_this_->sock, SHUT_RDWR);
}

int OeSockClient_send( T _this_, char *buf, size_t bsize ) {
    int rc;
    if (_this_->use_ssl) {
        rc = SSL_write(_this_->sslHandle, buf, bsize);
    } else {
        rc = send( _this_->sock, (void *) buf, bsize, 0);
    }
    return rc;
}

int OeSockClient_recv( T _this_, char *buf, size_t bsize ) {
    int rc;
    if (_this_->use_ssl) {
        rc = SSL_read (_this_->sslHandle, buf, bsize);
    } else {
        rc = recv( _this_->sock, buf, bsize, 0);
    }
    return rc;
}

#undef T

