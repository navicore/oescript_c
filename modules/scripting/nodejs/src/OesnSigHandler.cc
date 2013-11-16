#define BUILDING_NODE_EXTENSION
#include <v8.h>
#include <node.h>
#include <iostream>

#include <SignalHandler.h>
#include "OesnSigHandler.h"

#include "OesnDispatcher.h"
#include "OesnThreadDispatcher.h"
#include "OesnKernel.h"
#include "OesnJsonKernel.h"
#include "OesnStore.h"

#include <Oed_Dispatcher.h>
#include <Oe_Thread_Dispatcher.h>
#include <OEK.h>
#include <JsonKernel.h>
#include <OeStore.h>

using namespace node;
using namespace v8;

#define REQ_FUN_ARG(I, VAR)                             \
if (args.Length() <= (I) || !args[I]->IsFunction())     \
return ThrowException(Exception::TypeError(             \
String::New("Argument " #I " must be a function")));    \
Local<Function> VAR = Local<Function>::Cast(args[I]);


OesnSigHandler::OesnSigHandler(char *pidfilename, char *name, char *loglvl) {
    impl_ =  SignalHandler_new(pidfilename, name, loglvl);
};
OesnSigHandler::~OesnSigHandler() {
    SignalHandler_free(&impl_);
};

void OesnSigHandler::Init(Handle<Object> target) {

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("OesnSigHandler"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("OesnSigHandler"), constructor);
}

SignalHandler OesnSigHandler::getImpl() {
    return impl_;
}

Handle<Value> OesnSigHandler::New(const Arguments& args) {
    HandleScope scope;

    if (args.Length() != 3) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    char *pidfilename;
    if (args[0]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("bad arg 1")));
        return scope.Close(Undefined());
    } else {
        v8::Local<v8::String> s = args[0]->ToString();
        pidfilename = new char[s->Length() + 1];
        s->WriteAscii((char*)pidfilename);
    }

    char *name;
    if (args[1]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("bad arg 2")));
        return scope.Close(Undefined());
    } else {
        v8::Local<v8::String> s = args[1]->ToString();
        name = new char[s->Length() + 1];
        s->WriteAscii((char*)name);
    }

    char *loglvl;
    if (args[2]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("bad arg 3")));
        return scope.Close(Undefined());
    } else {
        v8::Local<v8::String> s = args[2]->ToString();
        loglvl = new char[s->Length() + 1];
        s->WriteAscii((char*)loglvl);
    }

    OesnSigHandler* obj = new OesnSigHandler(pidfilename, name, loglvl);
    obj->Wrap(args.This());

    return args.This();
}

