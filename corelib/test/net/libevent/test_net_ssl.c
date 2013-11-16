#include "config.h"
#undef NDEBUG
#include <assert.h>

#include <OeSockClient.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <event.h>
#include <event2/listener.h>
#include <event2/bufferevent_ssl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "OeNet.h"
#include "OeSession.h"
#include "OeLibevent2Factory.h"

int clisock;
SSL *sslHandle;
SSL_CTX *sslContext;

struct sockaddr_in servaddr;
struct hostent *hp;

int TEST_PORT = 8779;
const char *TEST_HOSTNAME = "localhost";

OeNet net;

void echo_buffered_on_read(OeSession client) {

    OeNetConn conn = OeSession_get_conn(client);
    struct bufferevent *bev = (struct bufferevent *)oenetconn_get_conn_obj(conn);

    char *peekdata = OeNetConn_peek(conn, 5, 5);
    int pos = OeNetConn_get_pos(conn, "cid:", 4);
    printf("peek: %s\n", peekdata);

    int SZ = 50;
    char input[SZ + 1];
    memset(&input, 0, sizeof(input));
    int rd_cnt = bufferevent_read(bev, input, SZ);
    printf("echo_buffered_on_read read %i: %s\n", rd_cnt, input);

    bufferevent_write(bev, "foo", 4);

    if ( strcmp("bye", input) == 0 ) {
        OE_ILOG(NULL,"bye. quitting.\n");
        event_loopexit(0);
    }
}

void test_start_listener(void) {

    OE_ILOG(NULL,"running ssl echo server...\n");

    Oed_Dispatcher dispatcher = Oed_Dispatcher_new(1, "async");
    net = OeLibevent2Factory_create(dispatcher, "cert", "pkey", OE_TCP_SSL);

    OeNet_set_read_handler(net, echo_buffered_on_read);
    OeNet_listen_for_connect( net, NULL, TEST_PORT );
    event_base_loop( Oed_Dispatcher_get_event_base(dispatcher), 0 );
    OeNet_stats( net );
    OeNet_stop( net );
    OeNet_free( &net );

    return;
}
void test_stop_listener(void) {

    OE_ILOG(NULL,"...stopping ssl echo server\n");

    char *buf = "bye";
    SSL_write(sslHandle, buf, strlen(buf));
}

const char *pass="password";
static int _password_cb(char *buf,int num,int rwflag, void *userdata) {
    printf("********* _password_cb ************* \n");
   if (num<strlen(pass)+1) return 0;
   strcpy(buf,pass);
   return(strlen(pass));
}

void test_start_client(void) {

    hp = gethostbyname("localhost");
    assert( hp );

    memset( &servaddr, 0, sizeof( servaddr ) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons( TEST_PORT );
    servaddr.sin_addr.s_addr = ((struct in_addr *) (hp->h_addr))->s_addr;

    for (;;) {
        clisock = socket( AF_INET, SOCK_STREAM, 0); //osx likes a new socket each try 
        assert( clisock != -1 );
        int rc = connect( clisock,
                          (struct sockaddr *)&servaddr, sizeof(servaddr));
        if (rc == 0) { //server up yet?
            break;
        }
        OE_ILOG(NULL,"echo server not connecting yet. rc: %i\n", rc);
        usleep(10000);
    }

    if (clisock) {

        SSL_load_error_strings ();
        SSL_library_init ();

        OpenSSL_add_all_ciphers();
  
        sslContext = SSL_CTX_new (SSLv23_client_method ());
        if (sslContext == NULL) ERR_print_errors_fp (stderr);
        SSL_CTX_set_verify(sslContext, SSL_VERIFY_NONE, NULL);
        SSL_CTX_set_options(sslContext, SSL_OP_NO_SSLv2);

        sslHandle = SSL_new (sslContext);
        if (sslHandle == NULL) ERR_print_errors_fp (stderr);

        if (!SSL_set_fd (sslHandle, clisock)) ERR_print_errors_fp (stderr);

        if (SSL_connect (sslHandle) != 1) ERR_print_errors_fp (stderr);

        SSL_set_mode(sslHandle, SSL_MODE_AUTO_RETRY);
        printf("test client cypher: %s\n", SSL_get_cipher_name(sslHandle));

    } else {
        perror ("Connect failed");
    }

    return;
}

void test_subject_api(void) {

    int SZ = 50;

    char *buf = "Hiya! My cid: 99 and my tid: 66\n\n";
    int wt_cnt = SSL_write (sslHandle, buf, strlen (buf));
    printf("test_subject_api wrote %i\n", wt_cnt);

    char response[SZ + 1];

    int received = SSL_read (sslHandle, response, SZ);
    response[received] = '\0';

    printf("test_subject_api received %i: %s\n", received, response);

    assert(strcmp("foo", response) == 0);
}

void test_2(void) {

    int SZ = 50;

    char *buf = "More Hiya.  My cid:999 my tid:666\n\n";
    SSL_write(sslHandle, buf, strlen(buf));

    char response[SZ + 1];

    SSL_read(sslHandle, response, SZ);

    assert(strcmp("foo", response) == 0);
}

void test_sock_client(void) {

    OeSockClient c = OeSockClient_new_ssl(TEST_HOSTNAME, TEST_PORT);
    int rc = OeSockClient_connect(c);
    assert(!rc);

    char *buf = "Mui Hiya";
    int len = strlen(buf);
    rc = OeSockClient_send(c, buf, len);
    assert(rc > 0);

    char response[len + 1];
    rc = OeSockClient_recv(c, response, len);
    assert(rc > 0);
    assert(strcmp("foo", response) == 0);

    OeSockClient_free(&c);
}

void test_tid_save_and_clear(void) {

    OeSession client = oesession_new_dummy();

    oe_id t1 = OeSession_clear_tid(client, (oe_id) 111);
    assert(!t1);

    OeSession_save_tid(client, (oe_id) 99);
    oe_id t2 = OeSession_clear_tid(client, (oe_id) 99);
    assert(t2 == 99);
}

int main(void) {

    pid_t pid = fork();
    assert(pid >= 0);

    if (!pid) {

        test_start_listener();

    } else {
        
        test_tid_save_and_clear();
        //wait for server to start

        test_start_client();

        //test1..n
        test_subject_api();
        test_2();
        test_2();
        test_2();
        test_2();
        test_subject_api();
        test_subject_api();
        int sz = 20000;
        oe_time start = oec_get_time_milliseconds();
        for (int i = 0; i < sz; i++) {
            test_2();
        }
        oe_time stop = oec_get_time_milliseconds();
        oe_time dur = stop - start;
        OE_ILOG(NULL, "oenet t2 dur: %lld size: %d\n", dur, sz );
        
        test_start_client();
        test_start_client();
        test_start_client();

        test_sock_client();

        usleep(10000);

        //cleanup
        test_stop_listener();
    }

    return 0;
}

