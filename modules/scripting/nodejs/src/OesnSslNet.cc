#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <iostream>
#include "OesnNet.h"
#include "OesnSslNet.h"
#include "OesnDispatcher.h"
#include <OeNet.h>
#include <OeNetDefaultFactory.h>

using namespace v8;

OesnSslNet::OesnSslNet(OesnDispatcher *d, char *cert, char *pkey) : OesnNet(d) {
    impl_ = OeNetDefaultSslFactory_create(d->getImpl(), cert, pkey);
};
OesnSslNet::~OesnSslNet() {
    //OeNet_free(&impl_);
};

void OesnSslNet::Init(Handle<Object> target) {

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("OesnSslNet"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("OesnSslNet"), constructor);
}

Handle<Value> OesnSslNet::New(const Arguments& args) {
    HandleScope scope;

    if (args.Length() != 3) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    OesnDispatcher* d = ObjectWrap::Unwrap<OesnDispatcher>(args[0]->ToObject());

    if (args[1]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("no cert")));
        return scope.Close(Undefined());
    }
    if (args[2]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("no pkey")));
        return scope.Close(Undefined());
    } 

    char *cert;
    v8::Local<v8::String> s = args[1]->ToString();
    cert = new char[s->Length() + 1];
    s->WriteAscii((char*)cert);

    char *pkey;
    s = args[2]->ToString();
    pkey = new char[s->Length() + 1];
    s->WriteAscii((char*)pkey);

    OesnSslNet* obj = new OesnSslNet(d, cert, pkey);
    obj->Wrap(args.This());

    delete cert;
    delete pkey;

    return args.This();
}

