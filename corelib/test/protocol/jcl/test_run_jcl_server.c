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
#include "SignalHandler.h"
#include "OeNet.h"
#include "OeNetDefaultFactory.h"

static Arena_T arena;
static Oed_Dispatcher dispatcher;
static OepServer oeserver;
static SignalHandler signals;
static char *URL = "oejcl://0.0.0.0:6789";

OEK oek_fnew(void) {

    OeStore store = OeBdb_new(1, "data", false);
    assert(store);

    Oed_Dispatcher matcher;
    matcher = Oed_Dispatcher_new(1, "kernel dispatcher");
    SignalHandler_manage(signals, 
                         Oed_Dispatcher_stop, 
                         Oed_Dispatcher_free, 
                         Oed_Dispatcher_stats, 
                         matcher);
    assert(matcher);
#ifdef OE_USE_THREADS
    Oed_Dispatcher_start(matcher);
#endif

    Oe_Thread_Dispatcher db_dispatcher = Oe_Thread_Dispatcher_new(1);
    assert(db_dispatcher);
    Oe_Thread_Dispatcher_start(db_dispatcher);
    SignalHandler_manage(signals, 
                         Oe_Thread_Dispatcher_stop, 
                         Oe_Thread_Dispatcher_free, 
                         Oe_Thread_Dispatcher_stats, 
                         db_dispatcher);

    OEK kernel = OEK_default_factory_new( matcher, db_dispatcher, store );
    assert(kernel);

    return kernel;
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

void start_socket_server() {

    signals = SignalHandler_new("my.pid", "mytest", "debug");

    OEK kernel = oek_fnew();
    dispatcher = Oed_Dispatcher_new(0, "jcl threads");
    SignalHandler_manage(signals, 
                         Oed_Dispatcher_stop, 
                         Oed_Dispatcher_free, 
                         Oed_Dispatcher_stats, 
                         dispatcher);
    OeNet net = OeNetDefaultFactory_create(dispatcher);
    assert(net);
    oeserver = OepFactory_new_server(dispatcher, URL, net, NULL);
    OepServer_add_kernel(oeserver, kernel, "myspace");
    Oed_Dispatcher_start(dispatcher);
}


int main() {

    arena = Arena_new();

    start_socket_server();

    return 0;
}

