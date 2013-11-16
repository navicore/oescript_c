#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <iostream>
#include "OesnNet.h"
#include "OesnDispatcher.h"
#include <OeNet.h>
#include <OeNetDefaultFactory.h>

using namespace v8;

OesnNet::OesnNet(OesnDispatcher *d) {
    impl_ = OeNetDefaultFactory_create(d->getImpl());
};
OesnNet::~OesnNet() {
    //OeNet_free(&impl_);
};

void OesnNet::Init(Handle<Object> target) {

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("OesnNet"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("setName"),
                                  FunctionTemplate::New(SetName)->GetFunction());

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("OesnNet"), constructor);
}

OeNet OesnNet::getImpl() {
    return impl_;
}

Handle<Value> OesnNet::New(const Arguments& args) {
    HandleScope scope;

    if (args.Length() != 1) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    OesnDispatcher* d = ObjectWrap::Unwrap<OesnDispatcher>(args[0]->ToObject());

    OesnNet* obj = new OesnNet(d);
    obj->Wrap(args.This());

    return args.This();
}

Handle<Value> OesnNet::SetName(const Arguments& args) {
    HandleScope scope;

    if (args.Length() != 1) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }
    if (args[0]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("no name")));
        return scope.Close(Undefined());
    }

    char *name;
    v8::Local<v8::String> s = args[0]->ToString();
    name = new char[s->Length() + 1];
    s->WriteAscii((char*)name);

    OesnNet* obj = ObjectWrap::Unwrap<OesnNet>(args.This());
    OeNet net = obj->impl_;
    OeNet_set_name(net, name);

    delete name;

    return args.This();
}

