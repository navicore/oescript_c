#include "config.h"
#undef NDEBUG
#include <assert.h>

#include "Oed_Dispatcher.h"
#include "Oe_Thread_Dispatcher.h"
#include "OeStore.h"
#include "OeBdb.h"
#include "OEK.h"
#include "JsonKernel.h"

JsonKernel jkernel;

static char *READ_CMD = "{                  \
	\"t\":	\"cmd\",                \
	\"n\":	\"get\",                    \
	\"cid\":	28,                         \
	\"timeout\":	50000,                  \
	\"c\":	1,                      \
	\"rattrs\":	true,               \
	\"templates\":	[[\"tel\", \"456\"]]    \
}";

static char *WRITE_CMD = "";

void _callmeback(char *json_res, void *arg) {
    assert(json_res);
    assert(arg);

    assert(strncmp((char *) arg, "hiyo", 4) != 0);
    assert(strncmp((char *) arg, "hiya", 4) == 0);
    void *sub = oe_memmem(json_res, strlen(json_res), "response", 8);
    assert(sub);
}

void test_1() {
    assert(true);
    int rc = JsonKernel_exec(jkernel, READ_CMD, _callmeback, "hiya");
    assert(!rc);
}

//suite
int main(void) {

    Oed_Dispatcher d = Oed_Dispatcher_new(1, "main");
    Oed_Dispatcher_start(d);
    //Oe_Thread_Dispatcher td = Oe_Thread_Dispatcher_new(1);
    Oe_Thread_Dispatcher td = Oe_Thread_Dispatcher_new(0);
    Oe_Thread_Dispatcher_start(td);
    OeStore store = OeBdb_new(1, "data", true);

    OEK kernel = OEK_default_factory_new(d, td, store);
    assert(kernel);

    jkernel = JsonKernel_new(kernel);
    assert(jkernel);

    test_1();
    //sleep(2);

    return 0;
}

