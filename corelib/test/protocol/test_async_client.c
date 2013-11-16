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
#undef NDEBUG
#include "config.h"
#include <event.h>
#include <assert.h>
#include "OeNet.h"
#include "OeNetConn.h"
#include "OeSession.h"
#include "OEK.h"
#include "OepClient.h"
#include "Oed_Dispatcher.h"
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include "OeNetDefaultFactory.h"

OeSession _client;

pid_t pid;
int clisock;
struct sockaddr_in servaddr;
struct hostent *hp;

bool connected = false;
OeNet net;
OepClient oep;
static Oed_Dispatcher dispatcher;
static int PORT = 7772;
static char *URI = "my://127.0.0.1:7772";
static oe_scalar USERNAME = "someguy";
static oe_scalar PWD = "secret";

void echo_buffered_on_read(OeSession client) {

    if (!client) return;
    struct bufferevent *bev = oenetconn_get_conn_obj(OeSession_get_conn(client));

    int SZ = 50;
    char input[SZ + 1];
    memset(&input, 0, sizeof(input));
    bufferevent_read(bev, input, SZ);
    printf("server read: %s\n", input);

    if (strcmp("bye", input) == 0) {
        OE_ILOG(NULL,".. actually stopping the echo server\n");
        event_loopexit(0);
        exit(0);
    }
    bufferevent_write(bev, "foo", 4);
}

void test_start_listener(void) {

    OE_ILOG(NULL,"running echo server...\n");

    Oed_Dispatcher dispatcher = Oed_Dispatcher_new(1, "async");
    net = OeNetDefaultFactory_create(dispatcher);

    OeNet_set_read_handler(net, echo_buffered_on_read);
    OeNet_listen_for_connect(net, NULL, PORT);
    event_base_loop( Oed_Dispatcher_get_event_base(dispatcher), 0 );
    OeNet_stop(net);
    OeNet_free(&net);

    return;
}
void test_stop_listener(void) {

    OE_ILOG(NULL,"trying to stop the echo server ...\n");

    struct bufferevent *bev = oenetconn_get_conn_obj(OeSession_get_conn(_client));
    bufferevent_write(bev, "bye", 4);
    kill(pid, SIGQUIT);
    kill(pid, SIGINT);
}

int _peek_msg(OeSession client, oe_id *cid) {
    struct bufferevent *bev = oenetconn_get_conn_obj(OeSession_get_conn(client));

    int SZ = 500;
    char input[SZ + 1];
    memset(&input, 0, sizeof(input));
    bufferevent_read(bev, input, SZ);

    if (!connected) {
        *cid = 1;
        return 1;
    }
    *cid = 0;
    return 0;
}

OekMsg _make_conres() {
    OekMsg resmsg = OekMsg_new(CON_RES, Arena_new());
    OekMsg_set_cid(resmsg, 1);
    ConRes res = OekMsg_get_con_res(resmsg);
    ConRes_set_sid(res, 99);
    return resmsg;
}

int YspNet_read_msg(OeSession client, OekMsg *msg, Arena_T arena) {
    struct bufferevent *bev = oenetconn_get_conn_obj(OeSession_get_conn(client));
    if (!connected) {
        connected = true;
        *msg = _make_conres();
        return 1;
    }
    return 0;
}

int YspNet_write_msg (OeSession client, OekMsg msg, Arena_T arena) {
    _client = client; //save for shutdown
    struct bufferevent *bev = oenetconn_get_conn_obj(OeSession_get_conn(client));
    bufferevent_write(bev, "foo", 4);
    return 0;
}

void test_start_client() {
    dispatcher = Oed_Dispatcher_new(1, "client threads");
    OeNet net = OeNetDefaultFactory_create(dispatcher);
    oep = OepClient_new(dispatcher, net, USERNAME, PWD, URI);
    assert(oep);
    OepClient_configure(oep, _peek_msg, YspNet_read_msg, YspNet_write_msg);
    Oed_Dispatcher_start(dispatcher);
    int rc = OepClient_connect(oep);
    assert(!rc);
}

void test_stop_client() {
    Oed_Dispatcher_stop(dispatcher);
    OEK kernel = OepClient_new_kernel(oep);
    void *aspp = OEK_get_client(kernel);
    assert(oep == aspp);
    OEK_free(&kernel);
    printf("nothing bad happened\n");
}

int main() {

    pid = fork();
    assert(pid >= 0);

    if (!pid) {

        test_start_listener();

    } else {
        usleep(10000);

        test_start_client();

        test_stop_listener();
        sleep(1);
        test_stop_client();
    }

    return 0;
}

