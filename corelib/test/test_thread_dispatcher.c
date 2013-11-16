#include "config.h"
#undef NDEBUG
#include <assert.h>

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "oe_common.h"
#include "Oe_Thread_Dispatcher.h"
#include "Oec_Future.h"
#include "Oec_CountdownLatch.h"

Arena_T arena;

Oe_Thread_Dispatcher dispatcher;
Oec_CountdownLatch latch;

void decrement_latch(void *dummy) {
    sleep(1);//sleep 1000 milliseconds
    Oec_CountdownLatch_decrement(latch);
}

Oec_Future future_name;
void set_my_name_in_the_future(void *name) {
    sleep(1);//sleep 1000 milliseconds
    Oec_Future_set(future_name, name);
}
char *say_my_name_in_the_future(void) {
    char *res;
    res = Oec_Future_get(future_name, 3500);
    return res;
}

char * makeString(char *str) {
    char *name_stor;
    name_stor = Arena_calloc(arena, 1, strlen(str) + 1, __FILE__, __LINE__);
    strcpy(name_stor, str);
    return name_stor;
}

bool say_my_name_ran_fast = false;
int say_my_name_count_fast = 0;
void say_my_name_fast(void *name) {
    //oe_log(NULL, "my name is %s.  my tid is %ld\n", name, pthread_self() );
    say_my_name_ran_fast = true;
    say_my_name_count_fast++;
    //usleep(2);
}

bool say_my_name_ran = false;
int say_my_name_count = 0;
void say_my_name(void *name) {
    //oe_log(NULL, "my name is %s.  my tid is %ld\n", name, pthread_self() );
    say_my_name_ran = true;
    say_my_name_count++;
    usleep(1000 * 2 * 2);
}

void test_dispatcher_init(void) {
    dispatcher = Oe_Thread_Dispatcher_new(1);
    assert(dispatcher);
    Oe_Thread_Dispatcher_start(dispatcher);
}

void test_dispatcher_test1(void) {

    int ITER = 8;

    oe_time start = oec_get_time_milliseconds();
    for (int i = 0; i < ITER; i++) {
        Oe_Thread_Dispatcher_exec(dispatcher, say_my_name, makeString("martha"));
    }
    usleep(1000 * 2 * 100);
    oe_time stop = oec_get_time_milliseconds();
    oe_time dur = stop - start;
    OE_ILOG(NULL, "thread dispatcher t1 dur: %lld size: %d\n", dur, say_my_name_count);
    assert( say_my_name_ran );
    assert( say_my_name_count == ITER);
}

void test_dispatcher_test2(void) {

    int ITER = 8000;

    oe_time start = oec_get_time_milliseconds();
    for (int i = 0; i < ITER; i++) {
        Oe_Thread_Dispatcher_exec(dispatcher, say_my_name_fast, makeString("mary"));
    }
    usleep(1000 * 100 * 10);
    oe_time stop = oec_get_time_milliseconds();
    oe_time dur = stop - start;
    OE_ILOG(NULL, "thread dispatcher t2 dur: %lld size: %d\n", dur, say_my_name_count_fast);
    assert( say_my_name_ran_fast );
    assert( say_my_name_count_fast == ITER);
}

void test_futures(void) {

    char *name = makeString("jen the hen");
    char *res;

    oe_time start = oec_get_time_milliseconds();
    Oe_Thread_Dispatcher_exec( dispatcher, set_my_name_in_the_future, name );

    res = say_my_name_in_the_future();
    oe_time now = oec_get_time_milliseconds();
    oe_time dur = now - start;
    printf("test_futures dur: %lld\n", dur);
    assert( res );
    assert( res == name);
    assert( dur >= 1000 ); //there is a sleep of 1 sec in the set... impl
}

void test_countdown_latch(void) {


    Oec_CountdownLatch_increment(latch);
    Oec_CountdownLatch_increment(latch);
    int c = Oec_CountdownLatch_increment(latch);
    //printf("latch size: %d\n", c);
    assert(c == 3);

    c = Oec_CountdownLatch_get(latch, 1000);
    assert(c <= 0); //1 means success, this test should fail since we have not decremented latch

    oe_time start = oec_get_time_milliseconds();
    Oe_Thread_Dispatcher_exec( dispatcher, decrement_latch, NULL );
    Oe_Thread_Dispatcher_exec( dispatcher, decrement_latch, NULL );
    Oe_Thread_Dispatcher_exec( dispatcher, decrement_latch, NULL );

    c = Oec_CountdownLatch_get(latch, OEC_FOREVER);
    assert(c == 1);
    oe_time now = oec_get_time_milliseconds();
    oe_time dur = now - start;
    //printf("test_countdown dur: %lld\n", dur);
    assert( dur >= 500 );
}

void test_dispatcher_stop(void) {
    Oe_Thread_Dispatcher_stop(dispatcher);
    Oe_Thread_Dispatcher_free(&dispatcher);
}

//suite
int main(void) {

    arena = Arena_new();

    future_name = Oec_Future_new();
    latch = Oec_CountdownLatch_new();

    test_dispatcher_init();

    test_dispatcher_test1();
    test_dispatcher_test2();
    test_futures();
    test_countdown_latch();

    test_dispatcher_stop();

    Arena_free(arena);
    Arena_dispose(&arena);
    Oec_Future_free(&future_name);

    return 0;
}

