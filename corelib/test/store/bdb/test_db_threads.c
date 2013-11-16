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

#include "DataObject.h"
#include "Iterator.h"
#include "DataObjectList.h"
#include "Oe_Thread_Dispatcher.h"
#include <mem.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "OeBdb.h"
#include "OeStore.h"
#include DB_HEADER

static char *bytes = "nonesense";
static char *morenonse = "somebad_prop_name_that_goes_on_and_on_and_on_for_ever_and_ever_and_does_not_stop_at_all_except_now"; 

OeStore store;
Oe_Thread_Dispatcher dispatcher;

void testPutOne(void *n) {

    char *name = (char *) n;

    Arena_T arena = Arena_new();
    printf("putting name: %s\n", (char *) name);

    OeStoreHolder holder = OeStoreHolder_new(arena);
    DataObject data = OeStoreHolder_get_data(holder);
    OeStoreHolder_set_txn_id( holder, 0 ); //unlocked
    OeStoreHolder_set_lease_expire_time( holder, oec_get_time_milliseconds() + 600000);
    OeStoreHolder_set_lock( holder, OESTORE_UNLOCKED );

    DataObject_set_nbytes( data, strlen(bytes) + 1 );
    DataObject_set_bytes( data, bytes );
    DataObject_add_attr( data, "key1" );
    DataObject_add_attr( data, (char *) name );
    DataObject_add_attr( data, "key2" );
    DataObject_add_attr( data, "seeme2three" );
    DataObject_add_attr( data, morenonse);
    DataObject_add_attr( data, "somebadvalue" );

    int ret;
    while ( (ret = OeStore_put( store, holder, arena)) ) {
        if (ret == OE_STORE_PLEASE_RETRY) {
            if (ret == OE_STORE_PLEASE_RETRY) {
                printf("put one retry for deadlock\n");
                continue;
            }
        }
    }
    assert(!ret);
    printf("put name: %s\n", (char *) name);
    Arena_dispose(&arena);
}

void testTakeOne(void *dummy) {

    Arena_T arena = Arena_new();
    const int MAX_RESULTS = 1;

    DataObjectList keylists = DataObjectList_new(arena);
    DataObject keys = DataObjectList_new_data_object(keylists);
    DataObject_add_attr(keys, "key2");
    DataObject_add_attr(keys, "seeme2three");

    OeStoreHolder results[MAX_RESULTS + 1];
    memset(results, 0, sizeof *results);
    size_t result_count = 0;

    OeStoreQuery query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, true);
    OeStoreQuery_set_tuple(query, false);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);

    int ret;
    while ( (
             ret = OeStore_get(store, query)
             ) ) {

        if (ret == OE_STORE_PLEASE_RETRY) {
            printf("take one retry for deadlock\n");
            OeStoreQuery_reset_nitems_found(query);
            continue;
        }
        if (ret == OE_STORE_ITEM_NOTFOUND) continue;
        result_count = OeStoreQuery_get_nitems_found(query);
        if (result_count == 0) continue;
    }
    assert(!ret);
    assert(result_count = 1);

    OeStoreHolder result = OeStoreQuery_get_store_holder(query, 0);
    DataObject data = OeStoreHolder_get_data(result);
    Iterator iter = DataObject_iterator(data, true);
    //Iterator iter = DataObject_iterator(data, false);
    printf("take result key1: %s\n", (char *) Iterator_next(iter)); //0
    printf("take result val1: %s\n", (char *) Iterator_next(iter)); //1
    printf("take result key2: %s\n", (char *) Iterator_next(iter)); //0
    assert(strcmp(Iterator_next(iter), "seeme2three") == 0);
    assert(strcmp(DataObject_get_bytes(data), "nonesense") == 0);
    assert(OeStoreHolder_get_lock(result) == OESTORE_TAKE_LOCK);
    Arena_dispose(&arena);
}


//suite
int main(void) {

#ifndef OE_USE_THREADS
    printf("can not test threads with no thread support compiled in\n");
    return 0;
#endif
    //arena = Arena_new();
    //dispatcher = Oe_Thread_Dispatcher_new(2);
    dispatcher = Oe_Thread_Dispatcher_new(1);
    assert(dispatcher);
    Oe_Thread_Dispatcher_start(dispatcher);
    sleep(1);

    store = OeBdb_new(1, "data", true);
    assert(store);

    char *tname;
    int sz = 10;
    for (int j = 0; j < sz; j++) {
        tname = Mem_alloc(100, __FILE__, __LINE__);
        sprintf(tname, "id:%d", j);
        Oe_Thread_Dispatcher_exec(dispatcher, testPutOne, tname);
    }
    for (int j = 0; j < sz; j++) {
        //Oe_Thread_Dispatcher_exec(dispatcher, testTakeOne, 0); //ejs test tmp
    }
    printf("waiting ........................................... \n");
    sleep(3);
    printf("........................................... done\n");
    Oe_Thread_Dispatcher_stop(dispatcher);
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    /// THE BUGS have to be related to query obj ///
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    //int close_ret = OeStore_free(&store); //why does this not like to stop?
    //Oe_Thread_Dispatcher_free(&dispatcher);
    //Arena_dispose(&arena);
}

/* experiment with this
#define RANGE   100000
void rand_sleep()
{ 
    pthread_t thrID  = pthread_self() + 1;
    unsigned seed  = (unsigned) getpid();
    seed  = seed * time(NULL);
    seed  = seed * ((unsigned)thrID);

    unsigned naptime=
    (unsigned)(1 + RANGE*((double)rand_r(&seed)/(double)RAND_MAX));
    setbuf(stdout,NULL);
    (void)usleep(naptime);
    return;
}
*/

