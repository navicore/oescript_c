/* The contents of this file are subject to the Apache License
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License 
 * from the file named COPYING and from http://www.apache.org/licenses/.
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * The Original Code is OeScript.
 *
 * The Initial Developer of the Original Code is OnExtent, LLC.
 * Portions created by OnExtent, LLC are Copyright (C) 2008-2011
 * OnExtent, LLC. All Rights Reserved.
 *
 */
#include "config.h"
#undef NDEBUG
#include <assert.h>

#include <stdbool.h>
#include <syslog.h>
#include "OepServer.h"
#include "OepFactory.h"
#include "OeStore.h"
#include "OeBdb.h"
#include "Oe_Thread_Dispatcher.h"
#include "Oed_Dispatcher.h"
#include "OEK.h"
#include "OEK_impl.h"
#include "OEK_default_factory.h"
#include "OeSockClient.h"
#include "OeNet.h"
#include "OeNetDefaultFactory.h"
#include "OeJclDecoder.h"
#include "OeJclEncoder.h"

static Arena_T arena;
static Oed_Dispatcher dispatcher;
static OEK _kernel;
static OepServer oeserver;
static OeSockClient client;
static int PORT = 4546;
static char *URL = "oejcl://0.0.0.0:4546";
static char *_extname = "myspace";

OEK oek_fnew(void) {

    openlog("oescript", LOG_PID | LOG_NOWAIT, LOG_LOCAL0);
    setlogmask(LOG_UPTO(LOG_DEBUG));

    OeStore store = OeBdb_new(1, "data", false);
    assert(store);

    Oed_Dispatcher matcher;
    matcher = Oed_Dispatcher_new(1, "kernel dispatcher");
    assert(matcher);
#ifdef OE_USE_THREADS
    Oed_Dispatcher_start(matcher);
#endif

    Oe_Thread_Dispatcher db_dispatcher = Oe_Thread_Dispatcher_new(1);
    assert(db_dispatcher);
    Oe_Thread_Dispatcher_start(db_dispatcher);

    _kernel = OEK_default_factory_new( matcher, db_dispatcher, store );
    assert(_kernel);

    return _kernel;
}

void oek_ffree(OEK *kernelp) {

    assert(kernelp);

    OEK kernel = *kernelp;
    assert(kernel);

    Oed_Dispatcher d = _get_matcher( kernel );
    assert(d);
    Oe_Thread_Dispatcher td = _get_db_dispatcher( kernel );
    assert(td);
    OeStore store = _get_store(kernel);
    assert(store);

    Oe_Thread_Dispatcher_stop( td );
    Oe_Thread_Dispatcher_free( &td );

    Oed_Dispatcher_stop( d );
    Oed_Dispatcher_free( &d );

    OeStore_free(&store);

    OEK_free(kernelp);
}

OekMsg _get_putcmd(oe_scalar key1, oe_scalar key2) {

    OekMsg msg = OekMsg_new(PUT_CMD, arena);
    OekMsg_set_cid(msg, 199);
    //OekMsg_set_tid(msg, 0);
    PutCmd cmd = OekMsg_get_put_cmd(msg);
    DataObject data = PutCmd_get_data(cmd);
    PutCmd_set_dur(cmd, 60000);
    char *bytes = "myputbytes123456789"; 
    DataObject_add_attr(data, key1);
    DataObject_add_attr(data, key2);
    DataObject_set_nbytes(data, strlen(bytes));
    DataObject_set_bytes(data, bytes);
    return msg;
}

OekMsg _get_getcmd(bool destroy, oe_scalar key1, oe_scalar key2) {

    OekMsg msg = OekMsg_new(GET_CMD, arena);
    OekMsg_set_cid(msg, 99);
    //OekMsg_set_tid(msg, 0);
    GetCmd cmd = OekMsg_get_get_cmd(msg);
    GetCmd_set_return_bytes(cmd, true);
    if (destroy) {
        GetCmd_set_remove_option(cmd);
    }
    //GetCmd_set_ifexists(cmd);
    GetCmd_set_return_attrs(cmd, true);
    GetCmd_set_max_responses(cmd, 13);
    GetCmd_set_timeout(cmd, 500);
   
    DataObject keys = GetCmd_new_data_object(cmd);
    DataObject_add_attr(keys, key1);
    DataObject_add_attr(keys, key2);
    /* 
    keys = GetCmd_new_keylist(cmd);
    DataObject_add_attr(keys, "my2key1");
    DataObject_add_attr(keys, "my2key2");
    DataObject_add_attr(keys, "my2key3"); 
    */ 
    
    return msg;
}

OekMsg _get_stacmd() {
    OekMsg msg = OekMsg_new(STA_CMD, arena);
    OekMsg_set_cid(msg, 100);
    StaCmd cmd = OekMsg_get_sta_cmd(msg);
    StaCmd_set_dur(cmd, 50000);
    return msg;
}

OekMsg _get_uptcmd() {
    OekMsg msg = OekMsg_new(UPT_CMD, arena);
    OekMsg_set_cid(msg, 101);
    OekMsg_set_tid(msg, 667);
    UptCmd cmd = OekMsg_get_upt_cmd(msg);
    UptCmd_set_dur(cmd, 5000);
    //UptCmd_set_dur(cmd, 0);
    UptCmd_set_status(cmd, 'C');
    return msg;
}

OekMsg _get_uplcmd() {
    OekMsg msg = OekMsg_new(UPL_CMD, arena);
    OekMsg_set_cid(msg, 545);
    UplCmd cmd = OekMsg_get_upl_cmd(msg);
    UplCmd_set_lid(cmd, 123);
    UplCmd_set_dur(cmd, 0);
    return msg;
}

OekMsg _get_concmd() {
    OekMsg msg = OekMsg_new(CON_CMD, arena);
    OekMsg_set_cid(msg, 919);
    ConCmd cmd = OekMsg_get_con_cmd(msg);
    ConCmd_set_extentname(cmd, (oe_scalar) "myspace");
    ConCmd_set_pwd(cmd, (oe_scalar) "secret");
    ConCmd_set_username(cmd, (oe_scalar) "myname");
    ConCmd_set_version_maj(cmd, 0);
    ConCmd_set_version_min(cmd, 3);
    ConCmd_set_version_upt(cmd, 7);
    return msg;
}

OekMsg _get_discmd() {
    OekMsg msg = OekMsg_new(DIS_CMD, arena);
    return msg;
}

static int write_handler(void *ext, unsigned char *buffer, size_t size) {
    OeSockClient_send(client, (char *) buffer, size);
    return 1;
}

void start_socket_client() {
    client = OeSockClient_new("127.0.0.1", PORT);
    int rc = OeSockClient_connect(client);
    assert(!rc);
}


int send_to_server(OekMsg *msgs) {

    for (int i = 0; msgs[i]; i++) {
        //int rc = jcl_msg_serialize(msgs[i], &buffer, OekMsg_get_arena(msgs[i]));
        Arena_T arena = OekMsg_get_arena(msgs[i]);
        OeJclEncoder encoder = OeJclEncoder_new(arena);
        char *buffer = OeJclEncoder_msg_serialize(encoder, msgs[i]);
        printf("sending buffer: %s\n", buffer);
        OeSockClient_send(client, buffer, strlen(buffer));
        OeSockClient_send(client, "\n\n", 2);
    }

    return 0;
}

//ejs todo: decode
int read_from_server(char *pattern) {

    char buffer[1000];
    int rc = OeSockClient_recv(client, buffer, 1000);
    if (rc > 0) {
        buffer[rc] = 0;
        printf("response:\n%s", buffer);
        if (strstr(buffer, pattern) <= 0) {
            return -1;
        } else {
            return 0;
        }
    } else {
        printf("no bytes\n");
    }
    return -1;
}

void start_socket_server() {
    _kernel = oek_fnew();
    dispatcher = Oed_Dispatcher_new(1, "jcl threads");
    Oed_Dispatcher_start(dispatcher);
    OeNet net = OeNetDefaultFactory_create(dispatcher);
    assert(net);
    oeserver = OepFactory_new_server(dispatcher, URL, net, NULL);
    OepServer_add_kernel(oeserver, _extname, _kernel);
}

void stop_socket_server() {
    OEK_stats(_kernel);
    OepServer_stats(oeserver);
    Oed_Dispatcher_stop(dispatcher);
    Oed_Dispatcher_free(&dispatcher);
    OepServer_free(&oeserver);
    closelog();
}

void test_con_2() {
    OekMsg msgs[4];
    msgs[0] = _get_concmd();
    msgs[1] = NULL;
    msgs[2] = NULL;
    send_to_server(msgs);
    //assert(!read_from_server("err:"));
    read_from_server("err:");
}

void test_con() {
    OekMsg msgs[4];
    msgs[0] = _get_concmd();
    //msgs[1] = _get_concmd(); //cause error by double connect
    msgs[1] = NULL;
    msgs[2] = NULL;
    send_to_server(msgs);
    //assert(!read_from_server("\"connect\""));
    assert(!read_from_server("cmd=connect"));
}

void test_unconnected_put() {
    OekMsg msgs[2];
    msgs[0] = _get_putcmd((oe_scalar) "one", (oe_scalar) "two");
    msgs[1] = NULL;
    send_to_server(msgs);

    //assert(!read_from_server("err:"));
    read_from_server("error:");
}
void test_put() {
    OekMsg msgs[2];
    msgs[0] = _get_putcmd((oe_scalar) "one", (oe_scalar) "two");
    msgs[1] = NULL;
    send_to_server(msgs);

    //assert(!read_from_server("\"put\""));
    assert(!read_from_server("cmd=write"));
}

void test_read() {
    OekMsg msgs[2];
    msgs[0] = _get_getcmd(false, (oe_scalar) "one", (oe_scalar) "two");
    msgs[1] = NULL;
    send_to_server(msgs);

    //assert(!read_from_server("\"get\""));
    assert(!read_from_server("cmd=read"));
}

void test_read_2() { //test for data return
    OekMsg msgs[2];
    msgs[0] = _get_getcmd(false, (oe_scalar) "one", (oe_scalar) "two");
    msgs[1] = NULL;
    send_to_server(msgs);

    //assert(!read_from_server("\"get\""));
    assert(!read_from_server("\"one\":"));
}

void test_take() {
    OekMsg msgs[2];
    msgs[0] = _get_getcmd(true, (oe_scalar) "one", (oe_scalar) "two");
    msgs[1] = NULL;
    send_to_server(msgs);

    //assert(!read_from_server("\"get\""));
    assert(!read_from_server("cmd=take"));
}

void test_upt() {
    OekMsg msgs[2];
    msgs[0] = _get_stacmd();
    msgs[1] = NULL;
    send_to_server(msgs);

    //assert(!read_from_server("\"sta\""));
    assert(!read_from_server("cmd=txn"));

    msgs[2];
    msgs[0] = _get_uptcmd();
    send_to_server(msgs);

    assert(read_from_server("error") == 0);
}

void test_upl() {
    OekMsg msgs[2];
    msgs[0] = _get_uplcmd();
    msgs[1] = NULL;
    send_to_server(msgs);

    assert(!read_from_server("lease not found"));
}

int main() {

    arena = Arena_new();

    start_socket_server();
    start_socket_client();
    test_unconnected_put(); //fatal, reconnect

    start_socket_client();
    test_con();
    test_con_2(); //fatal, reconnect

    start_socket_client();
    test_con();
    test_put();
    test_read();
    test_read_2();
    test_take();
    test_upt(); //fatal, reconnect

    start_socket_client();
    test_con();
    test_upl();

    stop_socket_server();

    return 0;
}

