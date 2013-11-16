#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <iostream>
#include "OesnStore.h"
#include <OeStore.h>
#include <OeStoreDefaultFactory.h>

using namespace v8;

OesnStore::OesnStore(int plevel, char *homedir, bool threaded) {
    impl_ = OeStoreDefaultFactory_create(plevel, homedir, threaded);
};
OesnStore::~OesnStore() {
    //OeStore_free(&impl_);
};

void OesnStore::Init(Handle<Object> target) {

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("OesnStore"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("OesnStore"), constructor);
}

OeStore OesnStore::getImpl() {
    return impl_;
}

Handle<Value> OesnStore::New(const Arguments& args) {
    HandleScope scope;

    if (args.Length() > 3) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    int plevel = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
    char *homedir;
    if (args[1]->IsUndefined()) {
        homedir = NULL;
    } else {
        v8::Local<v8::String> s = args[1]->ToString();
        homedir = new char[s->Length() + 1];
        s->WriteAscii((char*)homedir);
    }
    std::cout << "creating store with homedir '" << (homedir ? homedir : "<none>") << "\'\n";
    int threaded = args[2]->IsUndefined() ? 1 : args[2]->NumberValue();

    OesnStore* obj = new OesnStore(plevel, homedir, threaded);
    obj->Wrap(args.This());

    if (homedir) {
        delete homedir;
    }

    return args.This();
}

