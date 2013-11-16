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
#include <stdio.h>
#include <string.h>
#include "OeBdb.h"
#include "OeStore.h"
#include "Iterator.h"
#include DB_HEADER

const static bool TAKE_CMD = true;

Arena_T arena;
OeStore store;

int  get1           (OeStoreHolder *, char *);
int  put1           (oe_id, OESTORE_LOCK, char *);
int  take1          (oe_id, OeStoreHolder *, char *);
void testTxnCommitPut  ();
void testCommitTake ();
void testRollback   ();

int put1(oe_id tid, OESTORE_LOCK lock, char *key) {

    char *bytes = "txn_nonesense";

    OeStoreHolder holder = OeStoreHolder_new(arena);
    DataObject data = OeStoreHolder_get_data(holder);
    assert(holder);
    OeStoreHolder_set_txn_id( holder, tid );
    OeStoreHolder_set_lease_expire_time( holder, oec_get_time_milliseconds() + 6000);
    OeStoreHolder_set_lock( holder, lock );
    DataObject_set_nbytes( data, strlen(bytes) + 1 );
    DataObject_set_bytes( data, bytes );
    DataObject_add_attr(data, "key" );
    DataObject_add_attr(data, key );

    int rc = OeStore_put(store, holder, arena);
    return rc;
}

int get1(OeStoreHolder *holder_pp, char *key) {

    const int MAX_RESULTS = 1;

    DataObjectList keylists = DataObjectList_new(arena);
    DataObject keys = DataObjectList_new_data_object(keylists);
    DataObject_add_attr(keys, "key" );
    DataObject_add_attr(keys, key);

    OeStoreQuery query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, false);
    OeStoreQuery_set_tuple(query, false);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);
    int ret = OeStore_get(store, query);
    *holder_pp = OeStoreQuery_get_store_holder(query, 0);

    return ret;
}

int take1(oe_id tid, OeStoreHolder *holder_pp, char *key) {

    const int MAX_RESULTS = 1;

    DataObjectList keylists = DataObjectList_new(arena);
    DataObject keys = DataObjectList_new_data_object(keylists);
    DataObject_add_attr(keys, "key" );
    DataObject_add_attr(keys, key);

    OeStoreQuery query = OeStoreQuery_new(arena, MAX_RESULTS);
    OeStoreQuery_set_take(query, true);
    OeStoreQuery_set_tuple(query, false);
    OeStoreQuery_set_ifexists(query, false);
    OeStoreQuery_set_value_lists(query, keylists);
    OeStoreQuery_set_owner(query, tid);

    int ret = OeStore_get(store, query);
    *holder_pp = OeStoreQuery_get_store_holder(query, 0);

    return ret;
}

void testBogusTxnPut(void) {

    //start a txn
    OESTORE_TXN oetxn;
    oetxn.tid = 98765;
    //int ret = OeStore_start_txn(store, 10000, &oetxn, arena);

    int ret = put1(oetxn.tid, OESTORE_WRITE_LOCK, "wouldacoulda");
    assert(ret == OESTORE_INACTIVE_TXN);
}

void testTxnCommitPut(void) {

    //start a txn
    OESTORE_TXN oetxn;
    int ret = OeStore_start_txn(store, 10000, &oetxn, arena);
    assert(ret == 0);
    assert(oetxn.tid > 0);

    ret = put1(oetxn.tid, OESTORE_WRITE_LOCK, "txn_one");
    if (ret) {
        OE_ERR(NULL, "put1 failed: %s\n", db_strerror(ret));
    }
    assert(ret == 0);

    //make sure it isn't visible yet
    OeStoreHolder holder2;
    ret = get1(&holder2, "txn_one");
    assert(ret == OE_STORE_ITEM_NOTFOUND);

    //commit the txn
    ret = OeStore_update_txn(store, oetxn.tid, 0, OESTORE_TXN_COMMITTED, arena, -1);
    OE_ERR(NULL, "upt failed: %s\n", db_strerror(ret));
    assert(ret == 0);

    //make sure it IS visible now
    OeStoreHolder holder3;
    ret = get1(&holder3, "txn_one");
    DataObject data3 = OeStoreHolder_get_data(holder3);
    OE_ERR(NULL, "get1 failed: %s\n", db_strerror(ret));
    assert(!ret);
    Iterator iter = DataObject_iterator(data3, true);
    Iterator_next(iter);//0
    assert(strcmp(Iterator_next( iter), "txn_one") == 0);
}

//look up holder
void testCommitTake(void) {

    //start a txn
    OESTORE_TXN oetxn;
    int ret = OeStore_start_txn(store, 10000, &oetxn, arena);
    assert(ret == 0);

    OeStoreHolder holder;
    ret = take1(oetxn.tid, &holder, "txn_one");
    DataObject data = OeStoreHolder_get_data(holder);
    assert(!ret);
    assert(strcmp(DataObject_get_bytes( data ), "txn_nonesense") == 0);
    Iterator iter = DataObject_iterator(data, true);
    Iterator_next(iter);//0
    assert(strcmp(Iterator_next( iter ), "txn_one") == 0);

    //make sure it isn't visible anymore
    OeStoreHolder holder2;
    ret = get1( &holder2, "txn_one");
    assert(ret == OE_STORE_ITEM_NOTFOUND);

    //commit the txn
    ret = OeStore_update_txn(store, oetxn.tid, 0, OESTORE_TXN_COMMITTED, arena, -1);
    assert(ret == 0);

    //make sure it IS STILL not visible after commit of take
    OeStoreHolder holder3;
    ret = get1( &holder3, "txn_one");
    assert(ret == OE_STORE_ITEM_NOTFOUND);
}

// read/modify/write
void testCommitRMW(void) {

    //save new test data
    int ret = put1(0, OESTORE_UNLOCKED, "three");
    assert(ret == 0);

    //start a txn
    OESTORE_TXN oetxn;
    ret = OeStore_start_txn(store, 10000, &oetxn, arena);
    assert(ret == 0);

    //take the new data
    OeStoreHolder holder;
    ret = take1(oetxn.tid, &holder, "three");
    DataObject data = OeStoreHolder_get_data(holder);
    assert(!ret);
    Iterator iter = DataObject_iterator(data, true);
    assert(strcmp(Iterator_next( iter ), "key") == 0);
    assert(strcmp(Iterator_next( iter ), "three") == 0);
    assert(strcmp(DataObject_get_bytes( data ), "txn_nonesense") == 0);

    //make sure it isn't visible anymore
    OeStoreHolder holder2 = NULL;
    ret = get1( &holder2, "three");
    assert(ret == OE_STORE_ITEM_NOTFOUND);

    //put the replacement item into the space
    ret = put1(oetxn.tid, OESTORE_WRITE_LOCK, "four");
    assert(!ret);

    //commit the txn
    ret = OeStore_update_txn(store, oetxn.tid, 0,
                             OESTORE_TXN_COMMITTED, arena, 1); //wrong enlisted count
    assert(ret);
    ret = OeStore_update_txn(store, oetxn.tid, 0,
                             OESTORE_TXN_COMMITTED, arena, 2);//right enlisted count
    assert(!ret);

    //make sure four IS visible after commit
    OeStoreHolder holder3;
    OeStore_start_txn(store, 10000, &oetxn, arena);
    ret = take1(oetxn.tid, &holder3, "four"); //and clean up for next tests...
    DataObject data3 = OeStoreHolder_get_data(holder3);
    OeStore_update_txn(store, oetxn.tid, 0, OESTORE_TXN_COMMITTED, arena, -1);
    assert(!ret);
    iter = DataObject_iterator(data3, true);
    assert(strcmp(Iterator_next( iter ), "key") == 0);
    assert(strcmp(Iterator_next( iter ), "four") == 0);
    assert(strcmp(DataObject_get_bytes( data3 ), "txn_nonesense") == 0);

    //make sure three is NOT visible
    OeStoreHolder holder4;
    ret = get1(&holder4, "three");
    assert(ret == OE_STORE_ITEM_NOTFOUND);
}

//delete holder and object (take)
void testRollback(void) {

    //save new test data
    int ret = put1(0, OESTORE_UNLOCKED, "txn_two");
    assert(ret == 0);

    //start a txn
    OESTORE_TXN oetxn;
    ret = OeStore_start_txn(store, 10000, &oetxn, arena);
    assert(ret == 0);

    OeStoreHolder holder;
    ret = take1(oetxn.tid, &holder, "txn_two");
    DataObject data = OeStoreHolder_get_data(holder);
    assert(!ret);
    assert(holder);
    assert(strcmp(DataObject_get_bytes( data ), "txn_nonesense") == 0);
    Iterator iter = DataObject_iterator(data, true);
    Iterator_next(iter); //0
    assert(strcmp(Iterator_next( iter ), "txn_two") == 0);

    //make sure it isn't visible anymore
    OeStoreHolder holder2;
    ret = get1(&holder2, "txn_two");
    OE_ERR(NULL, "get1: %s\n", db_strerror(ret));
    assert(ret == OE_STORE_ITEM_NOTFOUND);
    ret = take1(0, &holder2, "txn_two");
    OE_ERR(NULL, "take1: %s\n", db_strerror(ret));
    assert(ret == OE_STORE_ITEM_NOTFOUND);

    //rollback the txn
    ret = OeStore_update_txn(store, oetxn.tid, 0, OESTORE_TXN_ROLLED_BACK, arena, -1);
    OE_ERR(NULL, "updatetxn: %s\n", db_strerror(ret));
    assert(ret == 0);

    //make sure it IS visible again after rollback of take
    OeStoreHolder holder4;
    ret = get1(&holder4, "txn_two");
    DataObject data4 = OeStoreHolder_get_data(holder4);
    OE_ERR(NULL, "get1: %s\n", db_strerror(ret));
    assert(!ret);
    iter = DataObject_iterator(data4, true);
    Iterator_next(iter); //0
    assert(strcmp(Iterator_next( iter ), "txn_two") == 0);
    OeStoreHolder holder3 = NULL;
    OeStore_start_txn(store, 10000, &oetxn, arena);
    ret = take1(oetxn.tid, &holder3, "txn_two"); //and clean up for next tests...
    DataObject data3 = OeStoreHolder_get_data(holder3);
    OE_ERR(NULL, "take1: %s\n", db_strerror(ret));
    assert(!ret);
    iter = DataObject_iterator(data3, true);
    Iterator_next(iter); //0
    assert(strcmp(Iterator_next( iter ), "txn_two") == 0);

    OeStore_update_txn(store, oetxn.tid, 0, OESTORE_TXN_COMMITTED, arena, 1);
}
void testDetectDeadlock(void) {

    OeStore_deadlock_detect(store, 1);
}

//suite
int main(void) {

    arena = Arena_new();

    store = OeBdb_new(1, "data", true);
    assert(store);

    testTxnCommitPut();
    testBogusTxnPut();
    testCommitTake();
    testCommitRMW();
    testRollback();

    for (int i = 0; i < 100; i++) {
        testCommitRMW();
    }
    testDetectDeadlock();

    OeStore_stats(store);
    int close_ret = OeStore_free(&store);
    assert(close_ret == 0);

    //test_update_of_expired_txn();
    //test_commit_of_expired_txn();

    Arena_free(arena);
    Arena_dispose(&arena);

    return 0;
}

