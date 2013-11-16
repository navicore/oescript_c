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
#include "DataObjectList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "OeBdb.h"
#include "OeStore.h"
#include "Iterator.h"
#include DB_HEADER

const static bool TAKE_CMD = true;
Arena_T arena;
OeStore store;

//insert an object
void testCommitPut(void) {

    char *bytes = "nonesense";

    OeStoreHolder holder = OeStoreHolder_new(arena);
    DataObject data = OeStoreHolder_get_data(holder);
    OeStoreHolder_set_txn_id( holder, 0 ); //unlocked
    OeStoreHolder_set_lease_expire_time( holder, oec_get_time_milliseconds() + 6000);
    OeStoreHolder_set_lock( holder, OESTORE_UNLOCKED );
    DataObject_set_nbytes( data, strlen(bytes) + 1 );
    DataObject_set_bytes( data, bytes );
    OeStoreHolder_set_tuple( holder, true );
    DataObject_add_attr( data, "seeme" );
    DataObject_add_attr( data, "seemetoo" );

    int ret = OeStore_put( store, holder, arena);

    assert(ret == 0);
}

//insert an object
void testPutOne(void) {

    char *bytes = "nonesense";

    OeStoreHolder holder = OeStoreHolder_new(arena);
    DataObject data = OeStoreHolder_get_data(holder);
    OeStoreHolder_set_txn_id( holder, 0 ); //unlocked
    OeStoreHolder_set_lease_expire_time( holder, oec_get_time_milliseconds() + 6000);
    OeStoreHolder_set_lock( holder, OESTORE_UNLOCKED );
    DataObject_set_nbytes( data, strlen(bytes) + 1 );
    DataObject_set_bytes( data, bytes );
    OeStoreHolder_set_tuple( holder, true );
    DataObject_add_attr( data, "seeme2" );
    DataObject_add_attr( data, "seeme2three" );
    DataObject_add_attr( data, "seemethridfld" );
    DataObject_add_attr( data, "somebad_prop_name_that_goes_on_and_on_and_on_for_ever_and_ever_and_does_not_stop_at_all_except_now" ); 
    DataObject_add_attr( data, "somebadpropvalue" ); 
    int ret = OeStore_put( store, holder, arena);

    assert(ret == 0);
}

//insert an object
void testPutOneLarge(void) {

    char *bytes = "nonesense"; 
    //char *bytes = "nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense nonesense and more nonesense";

    OeStoreHolder holder = OeStoreHolder_new(arena);
    DataObject data = OeStoreHolder_get_data(holder);
    OeStoreHolder_set_txn_id( holder, 0 ); //unlocked
    OeStoreHolder_set_lease_expire_time( holder, oec_get_time_milliseconds() + 6000);
    OeStoreHolder_set_lock( holder, OESTORE_UNLOCKED );
    DataObject_set_nbytes( data, strlen(bytes) + 1 );
    DataObject_set_bytes( data, bytes );
    OeStoreHolder_set_tuple( holder, true );
    DataObject_add_attr( data, "key" );
    DataObject_add_attr( data, "large1" );

    int ret = OeStore_put( store, holder, arena );

    assert(ret == 0);
}


//look up holder
void testGetLarge(void) {

    const int MAX_RESULTS = 1;

    DataObjectList keylists = DataObjectList_new(arena);
    DataObject keys = DataObjectList_new_data_object(keylists);
    DataObject_add_attr(keys, __OESTORE_WILDCARD__);
    DataObject_add_attr(keys, "large1");

    OeStoreQuery query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, false);
    OeStoreQuery_set_tuple(query, true);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);
    int ret = OeStore_get(store, query);

    if (ret) {
        OE_ERR(NULL, "get large failed: %s\n", db_strerror(ret));
    }
    assert(ret != DB_NOTFOUND);
    assert(ret == 0);
    size_t result_count = OeStoreQuery_get_nitems_found(query);
    assert(result_count == 1);

    OeStoreHolder result = OeStoreQuery_get_store_holder(query, 0);
    DataObject data = OeStoreHolder_get_data(result);
    assert(OeStoreHolder_get_lock(result) == OESTORE_UNLOCKED);
    Iterator iter = DataObject_iterator(data, true);
    assert(strcmp(Iterator_next(iter), "key") == 0);
    assert(strcmp(Iterator_next(iter), "large1") == 0);
    assert(strcmp(DataObject_get_bytes(data), "nonesense") == 0);
}

//look up holder
void testTakeLarge(void) {

    const int MAX_RESULTS = 1;

    DataObjectList keylists = DataObjectList_new(arena);
    DataObject keys = DataObjectList_new_data_object(keylists);
    DataObject_add_attr(keys, __OESTORE_WILDCARD__);
    DataObject_add_attr(keys, "large1");

    OeStoreQuery query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, true);
    OeStoreQuery_set_tuple(query, true);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);
    int ret = OeStore_get(store, query);

    assert(ret != DB_NOTFOUND);
    assert(ret == 0);
    size_t result_count = OeStoreQuery_get_nitems_found(query);
    assert(result_count == 1);

    OeStoreHolder result = OeStoreQuery_get_store_holder(query, 0);
    DataObject data = OeStoreHolder_get_data(result);
    assert(OeStoreHolder_get_lock(result) == OESTORE_TAKE_LOCK);
    Iterator iter = DataObject_iterator(data, true);
    assert(strcmp(Iterator_next(iter), "large1") == 0);
    assert(strcmp(DataObject_get_bytes(data), "nonesense") == 0);
}

//look up holder
void testGet(void) {

    const int MAX_RESULTS = 1;

    DataObjectList keylists = DataObjectList_new(arena);
    DataObject keys = DataObjectList_new_data_object(keylists);
    DataObject_add_attr(keys, "seeme");
    DataObject_add_attr(keys, "seemetoo");

    OeStoreQuery query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, false);
    OeStoreQuery_set_tuple(query, true);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);
    int ret = OeStore_get(store, query);

    assert(ret != DB_NOTFOUND);
    assert(ret == 0);
    size_t result_count = OeStoreQuery_get_nitems_found(query);
    assert(result_count == 1);

    OeStoreHolder result = OeStoreQuery_get_store_holder(query, 0);
    DataObject data = OeStoreHolder_get_data(result);
    assert(OeStoreHolder_get_lock(result) == OESTORE_UNLOCKED);
    Iterator iter = DataObject_iterator(data, true);
    assert(strcmp(Iterator_next(iter), "seeme") == 0);
    assert(strcmp(Iterator_next(iter), "seemetoo") == 0);
    assert(strcmp(DataObject_get_bytes(data), "nonesense") == 0);
}

//delete holder and object (take)
void testTake(void) {

    const int MAX_RESULTS = 1;

    DataObjectList keylists = DataObjectList_new(arena);
    DataObject keys = DataObjectList_new_data_object(keylists);
    DataObject_add_attr(keys, "seeme");
    DataObject_add_attr(keys, "seemetoo");

    OeStoreQuery query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, true);
    OeStoreQuery_set_tuple(query, true);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);
    int ret = OeStore_get(store, query);

    assert(ret != DB_NOTFOUND);
    assert(ret == 0);
    size_t result_count = OeStoreQuery_get_nitems_found(query);
    assert(result_count == 1);

    OeStoreHolder result = OeStoreQuery_get_store_holder(query, 0);
    DataObject data = OeStoreHolder_get_data(result);
    Iterator iter = DataObject_iterator(data, true);
    assert(strcmp(Iterator_next(iter), "seeme") == 0);
    assert(strcmp(Iterator_next(iter), "seemetoo") == 0);
    assert(strcmp(DataObject_get_bytes(data), "nonesense") == 0);
    assert(OeStoreHolder_get_lock(result) == OESTORE_TAKE_LOCK);

    result_count = 0; //reset

    //make sure the take took
    query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, false);
    OeStoreQuery_set_tuple(query, true);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);
    ret = OeStore_get(store, query);

    assert(ret == OE_STORE_ITEM_NOTFOUND);
    result_count = OeStoreQuery_get_nitems_found(query);
    assert(result_count == 0);
}

void testReadOne(void) {

    const int MAX_RESULTS = 1;

    DataObjectList keylists = DataObjectList_new(arena);
    DataObject keys = DataObjectList_new_data_object(keylists);
    DataObject_add_attr(keys, "seeme2");
    DataObject_add_attr(keys, "seeme2three");
    DataObject_add_attr(keys, __OESTORE_WILDCARD__);
    DataObject_add_attr(keys, __OESTORE_WILDCARD__);
    DataObject_add_attr(keys, __OESTORE_WILDCARD__);

    OeStoreQuery query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, false);
    OeStoreQuery_set_tuple(query, true);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);
    int ret = OeStore_get(store, query);

    assert(ret != DB_NOTFOUND);
    assert(ret == 0);
    size_t result_count = OeStoreQuery_get_nitems_found(query);
    assert(result_count == 1);

    OeStoreHolder result = OeStoreQuery_get_store_holder(query, 0);
    DataObject data = OeStoreHolder_get_data(result);
    assert(result);
    Iterator iter = DataObject_iterator(data, true);
    assert(strcmp(Iterator_next(iter), "seeme2") == 0);
    assert(strcmp(Iterator_next(iter), "seeme2three") == 0);
    assert(strcmp(Iterator_next(iter), "seemethridfld") == 0);
    assert(strcmp(DataObject_get_bytes(data), "nonesense") == 0);
    assert(OeStoreHolder_get_lock(result) == OESTORE_UNLOCKED);
}

void testTakeOne(void) {

    const int MAX_RESULTS = 1;

    DataObjectList keylists = DataObjectList_new(arena);
    DataObject keys = DataObjectList_new_data_object(keylists);
    DataObject_add_attr(keys, __OESTORE_WILDCARD__);
    DataObject_add_attr(keys, "seeme2three");
    DataObject_add_attr(keys, __OESTORE_WILDCARD__);
    DataObject_add_attr(keys, __OESTORE_WILDCARD__);
    DataObject_add_attr(keys, __OESTORE_WILDCARD__);

    OeStoreQuery query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, true);
    OeStoreQuery_set_tuple(query, true);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);
    int ret = OeStore_get(store, query);

    assert(ret != DB_NOTFOUND);
    assert(ret == 0);
    size_t result_count = OeStoreQuery_get_nitems_found(query);
    assert(result_count == 1);

    OeStoreHolder result = OeStoreQuery_get_store_holder(query, 0);
    DataObject data = OeStoreHolder_get_data(result);
    Iterator iter = DataObject_iterator(data, true);
    assert(strcmp(Iterator_next(iter), "seeme2") == 0);
    assert(strcmp(Iterator_next(iter), "seeme2three") == 0);
    assert(strcmp(DataObject_get_bytes(data), "nonesense") == 0);
    assert(OeStoreHolder_get_lock(result) == OESTORE_TAKE_LOCK);
}

int cleanup(void) {
    int ret = OeStore_stop(store);
    assert(ret == 0);
    ret = OeStore_free(&store);
    assert(ret == 0);

    Arena_free(arena);
    Arena_dispose(&arena);
    return 0;
}

//suite
int main(void) {

    arena = Arena_new();

    store = OeBdb_new(1, "data", true);
    assert(store);

    testCommitPut();

    for (int i = 0; i < 100; i++) {
        testGet();
    }
    Arena_free(arena);
    testTake();

    oe_time start = oec_get_time_milliseconds();
    int sz = 10;
    for (int j = 0; j < sz; j++) {

        for (int i = 0; i < sz; i++) {
            testPutOne();
        }

        for (int i = 0; i < sz; i++) {
            testReadOne();
            testTakeOne();
        }
        Arena_free(arena);
    }
    oe_time stop = oec_get_time_milliseconds();
    int ntxn = sz * sz * 3; //1 write 1 read 1 take
    OE_ILOG(NULL,"oedb t2 dur: %lld size: %d\n", stop - start, ntxn);
    start = oec_get_time_milliseconds();

    start = oec_get_time_milliseconds();
    //sz = 20000;
    sz = 200;
    for (int j = 0; j < sz; j++) {

        testPutOne();
        testReadOne();
        testTakeOne();
        Arena_free(arena);
    }
    stop = oec_get_time_milliseconds();
    ntxn = sz * 3;
    oe_time dur = stop - start;
    OE_ILOG(NULL, "oedb t3 dur: %lld size: %d\n", dur, ntxn );

    sz = 2000;
    for (int j = 0; j < sz; j++) {
        //testPutOne();
        testPutOneLarge();
    }
    for (int j = 0; j < sz; j++) {
        //testTakeOne();
        testGetLarge();               //this proves that when a cursor finds a lot of stuff the system dies
        //testTakeLarge();
    }

    return cleanup();
}

