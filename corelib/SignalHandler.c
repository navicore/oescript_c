/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <syslog.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "mem.h"
#include "list.h"
#include "SignalHandler.h"
#include <string.h>
#include "oescript_core_resources.h"
#include "oec_log.h"

#define T SignalHandler

static T _global_this_;

struct obj_ {

    user_sig_callback *shutdown;
    user_sig_callback *destroy;  //warning, note by convention _free calls get &arg, not arg
    user_sig_callback *diag;
    void *arg;
};

static void _sigint_handler(int sig) {
    OE_ILOG(0, OEC_SIGNALHANDLER_SIGINT);
    SignalHandler_shutdown(_global_this_);
    SignalHandler_free(&_global_this_);
    exit(0);
}

static void _sigquit_handler(int sig) {
    OE_ILOG(0,  OEC_SIGNALHANDLER_SIGQUIT);
    SignalHandler_diag(_global_this_);
    signal(SIGQUIT, _sigquit_handler);
}

static int getLogLevel(const char *lvl) {

    if (lvl == NULL || strcmp("debug", lvl) == 0)  return LOG_DEBUG;
    if (strcmp("info", lvl) == 0)  return LOG_INFO;
    if (strcmp("err", lvl) == 0)  return LOG_ERR;
    if (strcmp("warn", lvl) == 0)  return LOG_WARNING;
    if (strcmp("alert", lvl) == 0)  return LOG_ALERT; //basically turns logging off
    return LOG_INFO;
}

T SignalHandler_new(const char *pid_file_name,
                    const char *appname,
                    const char *debugLvl) {
    assert(!_global_this_); //can only have one of these
    T _this_;
    _this_ = Mem_alloc(sizeof *_this_, __FILE__, __LINE__);
    _global_this_ = _this_;

    _this_->objects = List_list(NULL);

    if (pid_file_name) {
        size_t len = strlen(pid_file_name);
        FILE *f = fopen(pid_file_name, "w");
        _this_->pid_file_name = Mem_alloc(len + 1, __FILE__, __LINE__);
        memcpy((char *) _this_->pid_file_name, pid_file_name, len + 1);
        int pid = getpid();
        fprintf(f, "%d\n", pid);
        fclose(f);
    }

    signal(SIGINT, _sigint_handler);
    signal(SIGQUIT, _sigquit_handler);

    openlog(appname == NULL ? "oescript" : appname, LOG_PID | LOG_NOWAIT, LOG_LOCAL0);
    setlogmask(LOG_UPTO(getLogLevel(debugLvl)));

    return _this_;
}

void SignalHandler_manage(T _this_, 
                          user_sig_callback *shutdown,
                          user_sig_callback *destroy,
                          user_sig_callback *diag,
                          void *arg ) {

    struct obj_ *o;
    o = Mem_alloc(sizeof *o, __FILE__, __LINE__);
    o->shutdown = shutdown;
    o->destroy = destroy;
    o->diag = diag;
    o->arg = arg;
    _this_->objects = List_append(_this_->objects, List_list(o, NULL));
}

void SignalHandler_shutdown(T _this_) {

    OE_ILOG(0, OEC_SIGNALHANDLER_SHUTDOWN);

    int len = List_length(_this_->objects);
    if (len > 0) {
        struct obj_ **objects = List_toArray(_this_->objects, NULL);
        for (int i = 0; i < len; i++) {
            user_sig_callback *myfun = (user_sig_callback*) objects[i]->shutdown;
            if (myfun) {
                myfun(objects[i]->arg);
            }
        }
        Mem_free(objects, __FILE__, __LINE__);
    }
}

static void _destroy(T _this_) {

    OE_ILOG(0, OEC_SIGNALHANDLER_DESTROY);

    int len = List_length(_this_->objects);
    if (len > 0) {
        struct obj_ **objects = List_toArray(_this_->objects, NULL);
        for (int i = 0; i < len; i++) {
            user_sig_callback *myfun = (user_sig_callback*) objects[i]->destroy;
            if (myfun) {
                myfun(&objects[i]->arg);
            }
        }
        Mem_free(objects, __FILE__, __LINE__);
    }
    closelog();
}

void SignalHandler_diag(T _this_) {

    OE_ILOG(0, OEC_SIGNALHANDLER_DIAG);

    int len = List_length(_this_->objects);
    if (len > 0) {
        struct obj_ **objects = List_toArray(_this_->objects, NULL);
        for (int i = 0; i < len; i++) {
            user_sig_callback *myfun = (user_sig_callback*) objects[i]->diag;
            if (myfun) {
                myfun(objects[i]->arg);
            }
        }
        Mem_free(objects, __FILE__, __LINE__);
    }
}

void SignalHandler_free(T* _this_p) {
    T _this_ = *_this_p;
    remove(_this_->pid_file_name);

    _destroy(_this_);

    List_free(&_this_->objects);
    Mem_free(_this_, __FILE__, __LINE__);
    *_this_p = NULL;
}

#undef T

