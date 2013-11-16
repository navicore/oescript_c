#include "config.h"
#undef NDEBUG
#include <assert.h>
#include "Oed_Dispatcher.h"
#include "SignalHandler.h"

Oed_Dispatcher dispatcher;
SignalHandler signals;

void test_dispatcher_init(void) {
    dispatcher = Oed_Dispatcher_new( 1, "rumple" );
    assert( dispatcher );
    Oed_Dispatcher_start( dispatcher );
    const char *name;
    Oed_Dispatcher_get_name(dispatcher, &name);
    printf("started dispatcher %s\n", name);
}

void test_sighandler_init(void) {
    signals = SignalHandler_new("my.pid", "mytest", "debug");
    assert( signals );
    SignalHandler_manage(signals, Oed_Dispatcher_stop, 
                         Oed_Dispatcher_free, Oed_Dispatcher_stats, dispatcher);
}

//suite
int main(void) {

    test_dispatcher_init();
    test_sighandler_init();

    SignalHandler_diag(signals);
    SignalHandler_shutdown(signals);
    SignalHandler_free(&signals);

    return 0;
}

