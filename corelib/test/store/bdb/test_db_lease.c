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

#include "Iterator.h"
#include "DataObject.h"
#include "DataObjectList.h"
#include <stdio.h>
#include <string.h>
#include "OeBdb.h"
#include "OeStore.h"
#include DB_HEADER

Arena_T arena;
OeStore store;

//insert an object
void testTxnCommitPut(void) {

    char *bytes = "nonesense";

    OeStoreHolder holder = OeStoreHolder_new(arena);
    DataObject data = OeStoreHolder_get_data(holder);
    OeStoreHolder_set_txn_id( holder, 0 ); //unlocked
    OeStoreHolder_set_lease_expire_time( holder, oec_get_time_milliseconds() + 6000);
    OeStoreHolder_set_lock( holder, OESTORE_UNLOCKED );
    DataObject_set_nbytes( data, strlen(bytes) + 1 );
    DataObject_set_bytes( data, bytes );
    DataObject_add_attr( data, "leaseme" );
    DataObject_add_attr( data, "leasemetoo" );

    assert(OeStoreHolder_get_lease_expire_time( holder ) > oec_get_time_milliseconds());

    int ret = OeStore_put(store, holder, arena);

    assert(ret == 0);
}

void testUpdateLease(void) {

    const int MAX_RESULTS = 1;

    DataObjectList keylists = DataObjectList_new(arena);
    DataObject keys = DataObjectList_new_data_object(keylists);
    DataObject_add_attr(keys, "leaseme");
    DataObject_add_attr(keys, "leasemetoo");

    //
    // (1) look to see what the current lease is
    //
    int sz = MAX_RESULTS + 1;

    size_t result_count;

    OeStoreQuery query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, false);
    OeStoreQuery_set_tuple(query, false);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);
    int ret = OeStore_get(store, query);

    assert(ret != OE_STORE_ITEM_NOTFOUND);
    assert(ret == 0);
    result_count = OeStoreQuery_get_nitems_found(query);
    assert(result_count == 1);

    OeStoreHolder r;
    r = OeStoreQuery_get_store_holder(query, 0);
    DataObject rdata = OeStoreHolder_get_data(r);
    assert(r);
    assert( OeStoreHolder_get_lock( r ) == OESTORE_UNLOCKED);
    Iterator iter = DataObject_iterator(rdata, true);
    assert(strcmp( Iterator_next( iter ), "leaseme") == 0);
    assert(strcmp( Iterator_next( iter ), "leasemetoo") == 0);
    assert(strcmp( DataObject_get_bytes( rdata ), "nonesense") == 0);

    //oe_time now = oec_get_time_milliseconds();
    //oe_time old_lease_expire_time = r->lease_expire_time;
    oe_time old_lease_expire_time = OeStoreHolder_get_lease_expire_time( r );

    //
    // (2) now update the lease
    //
    ret = OeStore_update_lease(store, OeStoreHolder_get_lease_id( r ),
                               oec_get_time_milliseconds() + 10000, arena);
    assert(ret == 0);

    //
    // (3) now look again to see that it changed
    //
    result_count = 0;

    query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, false);
    OeStoreQuery_set_tuple(query, false);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);
    ret = OeStore_get(store, query);
    assert(ret != OE_STORE_ITEM_NOTFOUND);
    assert(ret == 0);
    result_count = OeStoreQuery_get_nitems_found(query);
    assert(result_count == 1);

    r = OeStoreQuery_get_store_holder(query, 0);
    assert(OeStoreHolder_get_lease_expire_time( r ) > old_lease_expire_time);

    //
    // done.  cleanup by take
    //
    query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, true);
    OeStoreQuery_set_tuple(query, false);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);
    ret = OeStore_get(store, query);
    assert(ret != OE_STORE_ITEM_NOTFOUND);
    assert(ret == 0);

    query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, true);
    OeStoreQuery_set_tuple(query, false);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);
    ret = OeStore_get(store, query);
    assert(ret == OE_STORE_ITEM_NOTFOUND); //is it really gone?
}

//suite
int main(void) {

    arena = Arena_new();

    store = OeBdb_new(1, "data", true);
    assert(store);

    testTxnCommitPut();
    testUpdateLease();

    int close_ret = OeStore_free(&store);
    assert(close_ret == 0);

    Arena_free(arena);
    Arena_dispose(&arena);

    return 0;
}

