/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

#include "OepFactory.h"
#include "OepServer.h"
#include "OepClient.h"
#include "Oed_Dispatcher.h"
#include "OeStore.h"
#include "OEK.h"
#include "OeNet.h"
#include "JsonNet.h"
#include "OeJclNet.h"
#include "oep_utils.h"

/**
 * this factory wires oescript-native protocols to the 
 * oescript-native net implementation.  only add new protocols 
 * here that are in the oescript src and use the oescript net 
 * implementation. 
 *  
 * to implement non-oescript-native protocols, use the higher 
 * level apis and instantiate the kernel, dispatchers, and store 
 * and communicate with the kernel with the OekMsg api. 
 */

/**
 * server support
 */
static void _configure_json_server(OepServer as, OeStore store) {
    assert(as);
    OepServer_configure(as, JsonNet_read_msg, JsonNet_write_msg,
                        store == NULL ? NULL : (oe_id (*) (void *)) OeStore_create_id,
                        (void *) store);
}

static void _configure_jcl_server(OepServer as, OeStore store) {
    assert(as);
    OepServer_configure(as, OeJclNet_read_msg, OeJclNet_write_msg,
                        store == NULL ? NULL : (oe_id (*) (void *)) OeStore_create_id,
                        (void *) store);
}

OepServer OepFactory_new_server (Oed_Dispatcher dispatcher,
                                 char *spec,
                                 OeNet net,
                                 OeStore store) {

    assert(dispatcher);
    assert(spec);
    assert(net);
    //assert(store);
    OepServer netsrv = OepServer_new(dispatcher, net);
    assert(netsrv);

    if (strstr(spec, "oejson://") == spec) {
        _configure_json_server(netsrv, store);
    } else if (strstr(spec, "oejcl://") == spec) {
            _configure_jcl_server(netsrv, store);
    } else {
        assert(false); //todo: error
    }
    char *hostifc = oepu_get_hostname_from_spec(NULL, spec);
    if (!hostifc) assert(false); //todo: error
    int port = oepu_get_port_from_spec(spec);
    if (port <= 0) assert(false); //todo: error
    OepServer_listen(netsrv, hostifc, port);

    return netsrv;
}

/**
 * client support
 */
OepClient OepFactory_new_client(char *spec, Oed_Dispatcher dispatcher,
                                OeNet net, oe_scalar username, oe_scalar pwd) {

    OepClient client = OepClient_new(dispatcher, net, username, pwd, spec);
    if (strstr(spec, "oejson://") == spec) {
        OepClient_configure(client, JsonNet_peek_cid,
                            JsonNet_read_msg, JsonNet_write_msg);
    } else  if (strstr(spec, "oejcl://") == spec) {
            OepClient_configure(client, OeJclNet_peek_cid,
                                OeJclNet_read_msg, OeJclNet_write_msg);
    } else {
        assert(false);//todo error
    }
    int rc = OepClient_connect(client); //todo: should propably be callable
    if (rc) {
        //assert(false);
        return NULL;
    }
    return client;
}

