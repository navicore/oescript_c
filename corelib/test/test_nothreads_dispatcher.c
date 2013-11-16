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
    name_stor = Arena_calloc( arena, 1, strlen(str) + 1, __FILE__, __LINE__ );
    strcpy(name_stor, str);
    return name_stor;
}

void test_dispatcher_init(void) {
    dispatcher = Oed_Dispatcher_new( 0, "bumple" );
    assert( dispatcher );
}

sn_handler handler_1;
void i_want_to_shutdown( void *dp ) {
    Oed_Dispatcher d = (Oed_Dispatcher) dp;
    printf("got shutdown event\n");
    Oed_Dispatcher_stop(d);
}

//suite
int main(void) {

    OE_ILOG( NULL, "test_nothreads_dispatcher\n");

    arena = Arena_new();

    test_dispatcher_init();

    DataObject templ = DataObject_new(arena);
    DataObject_add_attr(templ, "i will never match");
    Oed_Dispatcher_reg( dispatcher, templ, 500,
                       false, NULL, i_want_to_shutdown, dispatcher );

    Oed_Dispatcher_start( dispatcher );

    Arena_free(arena);
    Arena_dispose(&arena);
    return 0;
}

