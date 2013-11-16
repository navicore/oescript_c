#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <iostream>
#include "OesnServer.h"
#include "OesnKernel.h"
#include "OesnLoginModule.h"
#include "OesnDispatcher.h"
#include "OesnNet.h"
#include "OesnSslNet.h"
#include "OesnStore.h"
#include <OepServer.h>
#include <OepFactory.h>

using namespace v8;

OesnServer::OesnServer(OesnDispatcher *d, char *spec, OesnNet *net, OesnStore *store) {
    impl_ = OepFactory_new_server(d->getImpl(), spec, net->getImpl(), store->getImpl());
};
OesnServer::~OesnServer() {
    //OepServer_free(&impl_);
};

void OesnServer::Init(Handle<Object> target) {

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("OesnServer"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("addLoginModule"),
                                  FunctionTemplate::New(AddLoginModule)->GetFunction());

    tpl->PrototypeTemplate()->Set(String::NewSymbol("addKernel"),
                                  FunctionTemplate::New(AddKernel)->GetFunction());

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("OesnServer"), constructor);
}

OepServer OesnServer::getImpl() {
    return impl_;
}

Handle<Value> OesnServer::New(const Arguments& args) {
    HandleScope scope;

    if (args.Length() != 4) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    OesnDispatcher* d = ObjectWrap::Unwrap<OesnDispatcher>(args[0]->ToObject());

    char *spec;
    if (args[1]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("no netspec arg")));
        return scope.Close(Undefined());
    } else {
        v8::Local<v8::String> s = args[1]->ToString();
        spec = new char[s->Length() + 1];
        s->WriteAscii((char*)spec);
    }
    OesnNet* net = ObjectWrap::Unwrap<OesnNet>(args[2]->ToObject());
    OesnStore* store = ObjectWrap::Unwrap<OesnStore>(args[3]->ToObject());

    OesnServer* obj = new OesnServer(d, spec, net, store);
    obj->Wrap(args.This());

    return args.This();
}

Handle<Value> OesnServer::AddLoginModule(const Arguments& args) {
    HandleScope scope;

    if (args.Length() != 1) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    OesnLoginModule* loginmod = ObjectWrap::Unwrap<OesnLoginModule>(args[0]->ToObject());

    OesnServer* obj = ObjectWrap::Unwrap<OesnServer>(args.This());
    OepServer_add_login_module(obj->getImpl(), loginmod->getImpl());

    return args.This();
}

Handle<Value> OesnServer::AddKernel(const Arguments& args) {
    HandleScope scope;

    if (args.Length() != 2) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    char *space;
    if (args[0]->IsUndefined()) {
        space = NULL;
    } else {
        v8::Local<v8::String> s = args[0]->ToString();
        space = new char[s->Length() + 1];
        s->WriteAscii((char*)space);
    }
    OesnKernel* k = ObjectWrap::Unwrap<OesnKernel>(args[1]->ToObject());

    OesnServer* obj = ObjectWrap::Unwrap<OesnServer>(args.This());
    OepServer_add_kernel(obj->getImpl(), space, k->getImpl());

    if (space) {
        delete space;
    }

    return args.This();
}

