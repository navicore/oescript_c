#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <iostream>
#include "OesnThreadDispatcher.h"
#include <Oe_Thread_Dispatcher.h>

using namespace v8;

OesnThreadDispatcher::OesnThreadDispatcher(int nthreads) {
    impl_ = Oe_Thread_Dispatcher_new(nthreads);
};
OesnThreadDispatcher::~OesnThreadDispatcher() {
    //Oe_Thread_Dispatcher_free(&impl_);
};

void OesnThreadDispatcher::Init(Handle<Object> target) {

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("OesnThreadDispatcher"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("start"),
                                  FunctionTemplate::New(Start)->GetFunction());

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("OesnThreadDispatcher"), constructor);
}

Oe_Thread_Dispatcher OesnThreadDispatcher::getImpl() {
    return impl_;
}

Handle<Value> OesnThreadDispatcher::New(const Arguments& args) {
    HandleScope scope;

    if (args.Length() > 1) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    int nthreads = args[0]->IsUndefined() ? 1 : args[0]->NumberValue();

    OesnThreadDispatcher* obj = new OesnThreadDispatcher(nthreads);
    obj->Wrap(args.This());

    return args.This();
}

Handle<Value> OesnThreadDispatcher::Start(const Arguments& args) {
    HandleScope scope;

    OesnThreadDispatcher* obj = ObjectWrap::Unwrap<OesnThreadDispatcher>(args.This());
    Oe_Thread_Dispatcher impl = obj->getImpl();
    Oe_Thread_Dispatcher_start(impl);
    return args.This();
}

