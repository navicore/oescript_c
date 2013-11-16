#include "config.h"
#undef NDEBUG
#include <assert.h>
#include <signal.h>

#include "oe_common.h"
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "Oed_Dispatcher.h"
#include "SignalHandler.h"
#include <sn_handler.h>

Arena_T arena;
Oed_Dispatcher dispatcher;
SignalHandler signals;

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
    signals = SignalHandler_new("my.pid", "mytest", "debug");
    SignalHandler_manage(signals, Oed_Dispatcher_stop, 
                         Oed_Dispatcher_free, Oed_Dispatcher_stats, dispatcher);
}

sn_handler handler_1;
void say_my_name_1( void *name ) {
    sn_handler_say( handler_1, name );
}

void test_dispatcher_exec_1(void) {

    int ITER = 8;

    for ( int i = 0; i < ITER; i++ ) {
        Oed_Dispatcher_exec( dispatcher, say_my_name_1, makeString("jenni") );
    }
    usleep(1000 * 2 * 100);
    assert( sn_handler_count( handler_1 ) == ITER);
}

sn_handler handler_t1;
void say_my_name_t1( void *name ) {
    printf("got it\n");
    sn_handler_say( handler_t1, name );
}

void test_dispatcher_timed_exec_1(void) {
    assert( sn_handler_count( handler_t1 ) == 0 );
    Oed_Dispatcher_timed_exec( dispatcher,
            say_my_name_t1, makeString("jo"), 400 );
    assert( sn_handler_count( handler_t1 ) == 0 );
    while (!sn_handler_count(handler_t1)) {
        sleep(1); //test doesn't actually die but will never end if there is a bug
    }
    //assert( sn_handler_count( handler_t1 ) == 1 );
}

sn_handler handler_2;
void say_my_name_2( void *name ) {
    sn_handler_say( handler_2, name );
}

void test_dispatcher_reg_toowildcard(void) {

    DataObject templ = DataObject_new(arena);
    DataObject_add_attr(templ, "_");
    DataObject_add_attr(templ, "_");
    //oe_id sid = 
    Oed_Dispatcher_reg(dispatcher, templ, 30000, false, say_my_name_2, say_my_name_2, makeString("wildest sam"));
}

void test_dispatcher_reg_wildcard(void) {

    DataObject templ = DataObject_new(arena);
    DataObject_add_attr(templ, "wildone");
    DataObject_add_attr(templ, "_");
    //oe_id sid = 
    Oed_Dispatcher_reg(dispatcher, templ, 30000, false, say_my_name_2, say_my_name_2, makeString("wild sam"));
}

void test_dispatcher_reg(void) {

    DataObject templ = DataObject_new(arena);
    DataObject_add_attr(templ, "one");
    DataObject_add_attr(templ, "two");
    //oe_id sid = 
    Oed_Dispatcher_reg(dispatcher, templ, 30000, false, say_my_name_2, say_my_name_2, makeString("sam"));
}

void test_dispatcher_try1(void) {

    DataObject templ = DataObject_new(arena);
    DataObject_add_attr(templ, "one");
    DataObject_add_attr(templ, "two");

    Oed_Dispatcher_process(dispatcher, templ, false);
}

void test_dispatcher_trywild1(void) {

    DataObject templ = DataObject_new(arena);
    DataObject_add_attr(templ, "wildone");
    DataObject_add_attr(templ, "two");

    Oed_Dispatcher_process(dispatcher, templ, false);
}

sn_handler ghandler1;
void say_my_name_g1(void *name) {
    sn_handler_say(ghandler1, name);
}

void test_dispatcher_reg_group(void) {

    DataObject templ = DataObject_new(arena);
    DataObject_add_attr(templ, "one");
    DataObject_add_attr(templ, "three");
    //oe_id sid = 
    Oed_Dispatcher_reg(dispatcher, templ, 30000, false, say_my_name_g1, NULL, makeString("sam"));
}

void test_dispatcher_try_group(void) {

    DataObject templ = DataObject_new(arena);
    DataObject_add_attr(templ, "one");
    DataObject_add_attr(templ, "three");
    DataObject_add_attr(templ, "ninetynine");

    Oed_Dispatcher_process(dispatcher, templ, false);

    usleep(.5 * 1000 * 1000);
    assert(sn_handler_ran(ghandler1));
}

sn_handler handler_4;
void say_my_name_4(void *name) {
    sn_handler_say(handler_4, name);
}

void test_dispatcher_try_timeout1(void) {

    sn_handler_reset(handler_4);
    DataObject templ = DataObject_new(arena);
    DataObject_add_attr(templ, "timeout");
    DataObject_add_attr(templ, "two");
    //oe_id sid = 
    Oed_Dispatcher_reg(dispatcher, templ, 100, false, say_my_name_4, say_my_name_4, makeString("sara"));

    assert(!sn_handler_ran(handler_4));

    usleep(.5 * 1000 * 1000);

    assert(sn_handler_ran(handler_4));
}

void test_dispatcher_mem_mgt(void) {

    sn_handler_reset(handler_4);
    assert(!sn_handler_ran(handler_4));

    int SZ = 1000;

    for (int i = 0; i < SZ; i++) {
        DataObject templ = DataObject_new(arena);
        DataObject_add_attr(templ, "timeout");
        DataObject_add_attr(templ, "two");
        Oed_Dispatcher_reg(dispatcher, templ, 2000, true, say_my_name_4, NULL, makeString("sara"));
    }
    for (int i = 0; i < SZ; i++) {
        DataObject templ = DataObject_new(arena);
        DataObject_add_attr(templ, "timeout");
        DataObject_add_attr(templ, "two");
        Oed_Dispatcher_process( dispatcher, templ, true );
    }

    usleep(.1 * 1000 * 1000);

    assert(sn_handler_ran(handler_4));
}

sn_handler handler_5;
void say_my_name_5(void *name) {
    sn_handler_say(handler_5, name);
}

void test_dispatcher_mem_mgt_no_hits(void) {

    int SZ = 1000;

    for (int i = 0; i < SZ; i++) {
        DataObject templ = DataObject_new(arena);
        DataObject_add_attr(templ, "timeout");
        DataObject_add_attr(templ, "two");
        Oed_Dispatcher_reg(dispatcher, templ, 100, true, say_my_name_5, NULL, makeString("sara"));
    }
    sleep(1);
    for (int i = 0; i < SZ; i++) {
        DataObject templ = DataObject_new(arena);
        DataObject_add_attr(templ, "timeout");
        DataObject_add_attr(templ, "two");
        Oed_Dispatcher_process( dispatcher, templ, true );
    }

    assert(!sn_handler_ran(handler_5));

    sleep(1);

    assert(!sn_handler_ran(handler_5));
}

sn_handler handler_fast;
void say_my_name_fast(void *name) {
    sn_handler_say(handler_fast, name);
}

void test_dispatcher_test2(void) {

    int ITER = 8000;

    oe_time start = oec_get_time_milliseconds();
    for (int i = 0; i < ITER; i++) {
        Oed_Dispatcher_exec(dispatcher, say_my_name_fast, makeString("mary"));
    }
    usleep(1000 * 100);
    oe_time stop = oec_get_time_milliseconds();
    oe_time dur = stop - start;
    OE_ILOG(NULL, "timer dispatcher t2 dur: %lld size: %d\n",
           dur, sn_handler_count(handler_fast));
    assert( sn_handler_ran(handler_fast) );
    assert( sn_handler_count(handler_fast) == ITER);
}

void test_dispatcher_stop(void) {
    Oed_Dispatcher_stop(dispatcher);
    Oed_Dispatcher_free(&dispatcher);
}

void test_names(void) {
    const char *name;
    Oed_Dispatcher_get_name(dispatcher, &name);

    assert(strcmp(name, "rumple") == 0);
}

sn_handler handler_6;
void say_my_name_6(void *name) {
    sn_handler_say(handler_6, name);
}

void test_dispatcher_many_templates(void) {

    //int SZ = 30000; //todo: debug why this can't be a higher value.  could it be max timers in libevent??
    int SZ = 15000;
    //int SZ = 3;

    for (int i = 0; i < SZ; i++) {
        DataObject templ = DataObject_new(arena);
        DataObject_add_attr( templ, makeString("update_name_request") );
        char *stri = Arena_calloc(arena, 1, 20 * sizeof(char), __FILE__, __LINE__);
        sprintf(stri, "subscription:%d", i);
        DataObject_add_attr( templ,  stri );
        oe_time dur =   20000300;
        Oed_Dispatcher_reg(dispatcher, templ, dur, true, say_my_name_6, NULL, makeString("sara jane")); 
    }
    sleep(1);
    DataObject templ = DataObject_new(arena);
    DataObject_add_attr( templ, makeString("update_name_request") );
    DataObject_add_attr( templ, makeString("subscription:1500") );
    DataObject_add_attr( templ, makeString("two") );
    DataObject_add_attr( templ, makeString("three") );
    DataObject_add_attr( templ, makeString("four") );
    DataObject_add_attr( templ, makeString("five") );
    oe_time starttime = oec_get_time_milliseconds();
    Oed_Dispatcher_process( dispatcher, templ, true );

    sleep(2);

    oe_time dur = sn_handler_timestamp(handler_6) - starttime;
    printf("...many template hits: %d\n", sn_handler_count(handler_6));
    assert(sn_handler_timestamp(handler_6));
    printf("...many template dur: %lld\n", dur);
    assert(dur < 1000); //should be < 2 but the test machine may be busy
    if (dur > 50) {
        printf("** warn, too slow many template **\n");
    }

    assert(sn_handler_ran(handler_6));
    assert(sn_handler_count(handler_6) == 1);

    sn_handler_reset(handler_6);
}

//suite
int main(void) {

//can't do any real testing with 0 threads
#ifdef OE_NOTHREADS
OE_ILOG(NULL, "warn, this test suite can't test dispatcher with OE_NOTHREADS defined\n");
#endif
#ifdef OE_USE_THREADS
    handler_1 = sn_handler_new();
    handler_t1 = sn_handler_new();
    handler_2 = sn_handler_new();
    ghandler1 = sn_handler_new();
    handler_4 = sn_handler_new();
    handler_5 = sn_handler_new();
    handler_6 = sn_handler_new();
    handler_fast = sn_handler_new();

    OE_ILOG( NULL, "test_timer_dispatcher\n");

    arena = Arena_new();

    test_dispatcher_init();

    test_dispatcher_timed_exec_1();

    test_dispatcher_many_templates();

    test_dispatcher_mem_mgt_no_hits();

    test_dispatcher_mem_mgt();

    test_dispatcher_exec_1();

    // test many--
    int c = 10000;
    for (int i = 0; i < c; i++) {
        test_dispatcher_reg();
        test_dispatcher_reg();
        test_dispatcher_reg();
        test_dispatcher_try1();
        test_dispatcher_try1();
        test_dispatcher_try1();
    }
    sleep(1);
    assert(sn_handler_count(handler_2) == c * 3);

    OE_ILOG( NULL, "test_timer_dispatcher many...\n");
    // --test many

    test_dispatcher_reg_group();
    test_dispatcher_try_group();

    test_dispatcher_try_timeout1();

    test_dispatcher_test2();

    //test many
    test_names();

    OE_ILOG( NULL, "test_timer_dispatcher wildcard\n");
    test_dispatcher_reg_wildcard();
    test_dispatcher_trywild1();
    test_dispatcher_reg_toowildcard();

    sleep(1); //can't free stuff while reg is in flight??

    //test signals
    //kill(getpid(), SIGQUIT);

    sn_handler_free(handler_1);
    sn_handler_free(handler_t1);
    sn_handler_free(handler_2);
    sn_handler_free(ghandler1);
    sn_handler_free(handler_4);
    sn_handler_free(handler_5);
    sn_handler_free(handler_6);
    sn_handler_free(handler_fast);
    test_dispatcher_stop();

    Arena_free(arena);
    Arena_dispose(&arena);
#endif
    return 0;
}

