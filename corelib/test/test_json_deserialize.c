#include "config.h"
#undef NDEBUG
#include <assert.h>
#include "arena.h"
#include "OekMsg.h"
#include "oec_file.h"
#include "cJSON.h"
#include "json_msg_factory.h"
#include <stdio.h>

static Arena_T arena;

static char *READ_CMD = "{                  \
	\"t\":	\"cmd\",                        \
	\"n\":	\"get\",                        \
	\"cid\":	28,                         \
	\"timeout\":	50000,                  \
	\"c\":	1,                              \
	\"rattrs\":	true,                       \
	\"templates\":	[[\"tel\", \"456\"], [\"seq\", 3, 99.1]]    \
}";

static char *READ_CMD2 = "{                 \
	\"t\":	\"cmd\",                        \
	\"n\":	\"get\",                        \
	\"cid\":	28,                         \
	\"timeout\":	50000,                  \
	\"c\":	1,                              \
	\"rattrs\":	true,                       \
	\"templates\":	[{\"mytel\": 456, \"myname\": \"me\", \"bal\": 10000.99}]    \
}";

int test_1() {

    OekMsg msg = NULL;

    int rc = json_msg_create(&msg, READ_CMD, arena);

    return rc;
}

int test_2() {

    OekMsg msg = NULL;

    int rc = json_msg_create(&msg, READ_CMD2, arena);

    return rc;
}

int main() {

    arena = Arena_new();

    assert(!test_1());
    assert(!test_2());

    return 0;
}

