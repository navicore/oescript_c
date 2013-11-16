#include "config.h"
#undef NDEBUG
#include <assert.h>

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "oe_common.h"
#include "oec_values.h"
#include <event.h>

int called = 0;

#define NEVENT	20000

struct event *ev[NEVENT];

/*
static int rand_int(int n) {
#ifdef WIN32
    return(int)(rand() * n);
#else
    return(int)(random() % n);
#endif
}
*/

static void
time_cb(int fd, short event, void *arg)
{
    //struct timeval tv;
    //int i, j;

    called++;
/*
    if (called < 10*NEVENT) {
        for (i = 0; i < 10; i++) {
            j = rand_int(NEVENT);
            tv.tv_sec = 0;
            tv.tv_usec = rand_int(50000);
            if (tv.tv_usec % 2)
                evtimer_add(ev[j], &tv);
            else
                evtimer_del(ev[j]);
        }
    }
*/
}

int
main (int argc, char **argv)
{
    struct timeval tv;
    int i;
    int sleep = 1;

    /* Initalize the event library */
    event_init();

    for (i = 0; i < NEVENT; i++) {
        ev[i] = malloc(sizeof(struct event));

        /* Initalize one event */
        evtimer_set(ev[i], time_cb, ev[i]);
        //tv.tv_sec = sleep;
        //tv.tv_usec = 0;

        tv.tv_sec = 0;
        tv.tv_usec = sleep * 1000;

        //tv.tv_sec = 0;
        //tv.tv_usec = rand_int(50000);

        evtimer_add(ev[i], &tv);
    }

    oe_time start = oec_get_time_milliseconds();
    event_dispatch();
    oe_time stop = oec_get_time_milliseconds();
    oe_time dur = stop - start;

    OE_ILOG( NULL, "test_libevent called: %i dur: %lld\n", called, dur);

    assert(dur >= sleep);
    assert(called == NEVENT);

    return(called < NEVENT);
}

