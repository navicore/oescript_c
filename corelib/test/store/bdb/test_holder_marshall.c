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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "OeStore.h"
#include "oec_values.h"
#include "Iterator.h"

Arena_T arena;

static oe_time TEST_TIME = 600060006000;

void testMarshall(void) {

    OeStoreHolder holder = OeStoreHolder_new(arena);
    DataObject data = OeStoreHolder_get_data(holder);
    OeStoreHolder_set_txn_id( holder, 0 ); //unlocked
    OeStoreHolder_set_lease_expire_time( holder, oec_get_time_milliseconds() + 6000);
    OeStoreHolder_set_lock( holder, OESTORE_WRITE_LOCK );
    DataObject_set_nbytes( data, strlen("hiya") );
    DataObject_set_bytes( data, "hiya" );
    DataObject_add_attr(data, "seeme" );
    DataObject_add_attr(data, "seemetoo" );

    char *buffer;
    int buffer_len;
    OeStoreHolder_marshal(holder, &buffer, &buffer_len, arena);

    OeStoreHolder new_holder = OeStoreHolder_new_from_buffer(arena, buffer);
    DataObject new_data = OeStoreHolder_get_data(new_holder);

    assert(OeStoreHolder_get_lock(holder) == OESTORE_WRITE_LOCK);
    assert(OeStoreHolder_get_lock(new_holder) == OESTORE_WRITE_LOCK);

    Iterator iter = DataObject_iterator(data, true);
    assert(strcmp(Iterator_next(iter), "seeme") == 0);
    iter = DataObject_iterator(new_data, true);
    assert(strcmp(Iterator_next(iter), "seeme") == 0);
}

void testPeekTxnId(void) {

    OeStoreHolder holder = OeStoreHolder_new(arena);
    DataObject data = OeStoreHolder_get_data(holder);
    OeStoreHolder_set_txn_id( holder, 1099 );
    OeStoreHolder_set_lease_expire_time( holder, oec_get_time_milliseconds() + 6000);
    OeStoreHolder_set_lock( holder, OESTORE_WRITE_LOCK );
    DataObject_set_nbytes( data, strlen("hiya") );
    DataObject_set_bytes( data, "hiya" );
    DataObject_add_attr(data, "seemetoo" );

    char *buffer;
    int buffer_len;
    OeStoreHolder_marshal(holder, &buffer, &buffer_len, arena);

    assert(OeStoreHolder_peek_txn_id(buffer) == 1099);
}

void testPeekExp(void) {

    OeStoreHolder holder = OeStoreHolder_new(arena);
    DataObject data = OeStoreHolder_get_data(holder);
    OeStoreHolder_set_txn_id( holder, 1099 );
    OeStoreHolder_set_lease_expire_time( holder, TEST_TIME);
    OeStoreHolder_set_lock( holder, OESTORE_WRITE_LOCK );
    DataObject_set_nbytes( data, strlen("hiya") );
    DataObject_set_bytes( data, "hiya" );
    DataObject_add_attr(data, "seeme" );

    char *buffer;
    int buffer_len;
    OeStoreHolder_marshal(holder, &buffer, &buffer_len, arena);

    assert(OeStoreHolder_peek_lease_exp_time(buffer) == TEST_TIME);
}

void testQueryHolder(void) {

    OeStoreQuery query = OeStoreQuery_new(arena, 50);

    OeStoreHolder holder = OeStoreHolder_new(arena);
    DataObject data = OeStoreHolder_get_data(holder);
    OeStoreHolder_set_txn_id( holder, 1099 );
    OeStoreHolder_set_lease_expire_time( holder, TEST_TIME);
    OeStoreHolder_set_lock( holder, OESTORE_WRITE_LOCK );
    DataObject_set_nbytes( data, strlen("hiya") );
    DataObject_set_bytes( data, "hiya" );
    DataObject_add_attr(data, "seeme" );

    assert(OeStoreQuery_get_nitems_found(query) == 0);
    OeStoreQuery_add_store_holder(query, holder);
    assert(OeStoreQuery_get_nitems_found(query) == 1);

    char *buffer;
    int buffer_len;
    OeStoreHolder_marshal(holder, &buffer, &buffer_len, arena);

    assert(OeStoreHolder_peek_lease_exp_time(buffer) == TEST_TIME);
}

int cleanup(void) {

    Arena_free(arena);
    Arena_dispose(&arena);
    return 0;
}

//suite
int main(void) {

    arena = Arena_new();

    for (int i = 0; 1000 > i; i++) {
        testMarshall();
    }
    testPeekTxnId();
    testPeekExp();
    testQueryHolder();

    return cleanup();
}

