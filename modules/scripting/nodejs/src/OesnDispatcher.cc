#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <iostream>
#include "OesnDispatcher.h"
#include <Oed_Dispatcher.h>

using namespace v8;

OesnDispatcher::OesnDispatcher(int nthreads, char *name) {
    impl_ = Oed_Dispatcher_new(nthreads, name);
};
OesnDispatcher::~OesnDispatcher() {
    //Oed_Dispatcher_free(&impl_);
};

void OesnDispatcher::Init(Handle<Object> target) {

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("OesnDispatcher"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("start"),
                                  FunctionTemplate::New(Start)->GetFunction());

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("OesnDispatcher"), constructor);
}

Oed_Dispatcher OesnDispatcher::getImpl() {
    return impl_;
}

Handle<Value> OesnDispatcher::New(const Arguments& args) {
    HandleScope scope;

    if (args.Length() != 2) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    int nthreads = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
    char *name;
    if (args[1]->IsUndefined()) {
        name = NULL;
    } else {
        v8::Local<v8::String> s = args[1]->ToString();
        name = new char[s->Length() + 1];
        s->WriteAscii((char*)name);
    }
    std::cout << "creating dispatcher named '" << (name ? name : "<none>") << "\'\n";

    OesnDispatcher* obj = new OesnDispatcher(nthreads, name);
    obj->Wrap(args.This());

    if (name) {
        delete name;
    }

    return args.This();
}

Handle<Value> OesnDispatcher::Start(const Arguments& args) {
    HandleScope scope;

    OesnDispatcher* obj = ObjectWrap::Unwrap<OesnDispatcher>(args.This());
    Oed_Dispatcher impl = obj->getImpl();
    Oed_Dispatcher_start(impl);
    return args.This();
}

