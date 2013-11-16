#include "config.h"
#undef NDEBUG
#include <assert.h>

#include "oe_common.h"
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "Oed_Dispatcher.h"
#include <sn_handler.h>

Arena_T arena;
Oed_Dispatcher dispatcher;

char * makeString(char *str) {
    char *name_stor;
    //name_stor = Arena_calloc( arena, 1, strlen(str) + 1, __FILE__, __LINE__ );
    name_stor = Mem_calloc( 1, strlen(str) + 1, __FILE__, __LINE__ );
    strcpy(name_stor, str);
    return name_stor;
}

void test_dispatcher_init(void) {
    dispatcher = Oed_Dispatcher_new( 1, "rumple" );
    assert( dispatcher );
    Oed_Dispatcher_start( dispatcher );
}

sn_handler ghandler1;
void say_my_name_g1(void *name) {
    sn_handler_say(ghandler1, name);
}

void test_dispatcher_reg_group(void) {

    DataObjectList group = DataObjectList_new(arena);
    DataObject templ = DataObjectList_new_data_object(group);
    DataObject_add_attr(templ, "one");
    DataObject_add_attr(templ, "three");

    oe_id sid = 
    Oed_Dispatcher_reg_group(dispatcher, group, 30000,
                       false, say_my_name_g1, NULL, makeString("sam"));
    assert(sid);
}

void test_dispatcher_try_group(void) {

    assert(!sn_handler_ran(ghandler1));
    DataObject templ = DataObject_new(arena);
    DataObject_add_attr(templ, "one");
    DataObject_add_attr(templ, "three");
    DataObject_add_attr(templ, "ninetynine");

    Oed_Dispatcher_process(dispatcher, templ, false);

    usleep(.5 * 1000 * 1000);
    assert(sn_handler_ran(ghandler1));
}

sn_handler ghandler2;
void say_my_name_g2(void *name) {
    sn_handler_say(ghandler2, name);
}

void test_dispatcher_reg_group2(void) {

    DataObjectList group = DataObjectList_new(arena);

    DataObject templ = DataObjectList_new_data_object(group);
    DataObject_add_attr(templ, "five");
    DataObject_add_attr(templ, "six");
    DataObject_add_attr(templ, "seven");
    DataObject_add_attr(templ, "eight");
    DataObject_add_attr(templ, "nine");

    templ = DataObjectList_new_data_object(group);
    DataObject_add_attr(templ, "four");
    DataObject_add_attr(templ, "five");

    templ = DataObjectList_new_data_object(group);
    DataObject_add_attr(templ, "hitme");

    oe_id sid = 
    Oed_Dispatcher_reg_group(dispatcher, group, 30000,
                       false, say_my_name_g2, NULL, makeString("sammy"));
    assert(sid);
}

void test_dispatcher_try_group2(void) {

    assert(!sn_handler_ran(ghandler2));

    DataObject templ = DataObject_new(arena);
    DataObject_add_attr(templ, "total");
    DataObject_add_attr(templ, "none");
    DataObject_add_attr(templ, "sense");

    Oed_Dispatcher_process(dispatcher, templ, false);
    usleep(.5 * 1000 * 1000);
    assert(!sn_handler_ran(ghandler2));

    templ = DataObject_new(arena);
    DataObject_add_attr(templ, "four");
    DataObject_add_attr(templ, "five");
    DataObject_add_attr(templ, "ninetynine");

    Oed_Dispatcher_process(dispatcher, templ, false);
    usleep(.5 * 1000 * 1000);

    assert(sn_handler_ran(ghandler2));
    assert(sn_handler_count(ghandler2) == 1);

    templ = DataObject_new(arena);
    DataObject_add_attr(templ, "hitme");
    Oed_Dispatcher_process(dispatcher, templ, false);
    usleep(.5 * 1000 * 1000);

    assert(sn_handler_count(ghandler2) == 1);
}

void test_dispatcher_stop(void) {
    Oed_Dispatcher_stop(dispatcher);
    Oed_Dispatcher_free(&dispatcher);
}

//suite
int main(void) {

//can't do any real testing with 0 threads
#ifdef OE_NOTHREADS
OE_ILOG(NULL, "warn, this test suite can't test dispatcher with OE_NOTHREADS defined\n");
#endif
#ifdef OE_USE_THREADS

    OE_ILOG( NULL, "test_group_dispatcher\n");

    arena = Arena_new();

    test_dispatcher_init();

    ghandler1 = sn_handler_new();
    test_dispatcher_reg_group();
    test_dispatcher_try_group();
    sn_handler_free(ghandler1);

    ghandler2 = sn_handler_new();
    test_dispatcher_reg_group2();
    test_dispatcher_try_group2();
    sn_handler_free(ghandler2);

    test_dispatcher_stop();

    Arena_free(arena);
    Arena_dispose(&arena);
#endif
    return 0;
}

