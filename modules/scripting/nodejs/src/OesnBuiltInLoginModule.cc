#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <iostream>
#include "OesnBuiltInLoginModule.h"
#include <json_msg_factory.h>
#include <BuiltInLoginModule.h>

using namespace v8;

OesnBuiltInLoginModule::OesnBuiltInLoginModule() : OesnLoginModule() {
    biimpl_ = BuiltInLoginModule_new();
    impl_ = BuiltInLoginModule_new_module(biimpl_);
};
OesnBuiltInLoginModule::~OesnBuiltInLoginModule() {
    //LoginModule_free(&impl_);
};

void OesnBuiltInLoginModule::Init(Handle<Object> target) {

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("OesnBuiltInLoginModule"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("addAccount"),
                                  FunctionTemplate::New(AddAccount)->GetFunction());

    tpl->PrototypeTemplate()->Set(String::NewSymbol("addPrivilege"),
                                  FunctionTemplate::New(AddPrivilege)->GetFunction());

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("OesnBuiltInLoginModule"), constructor);
}

Handle<Value> OesnBuiltInLoginModule::New(const Arguments& args) {
    HandleScope scope;

    OesnBuiltInLoginModule* obj = new OesnBuiltInLoginModule();
    obj->Wrap(args.This());

    return args.This();
}

Handle<Value> OesnBuiltInLoginModule::AddAccount(const Arguments& args) {
    HandleScope scope;

    if (args.Length() != 3) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    if (args[0]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("no group")));
        return scope.Close(Undefined());
    }
    if (args[1]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("no username")));
        return scope.Close(Undefined());
    }
    if (args[2]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("no password")));
        return scope.Close(Undefined());
    }
    char *group;
    v8::Local<v8::String> s = args[0]->ToString();
    group = new char[s->Length() + 1];
    s->WriteAscii((char*)group);

    char *username;
    s = args[1]->ToString();
    username = new char[s->Length() + 1];
    s->WriteAscii((char*)username);

    char *pwd;
    s = args[2]->ToString();
    pwd = new char[s->Length() + 1];
    s->WriteAscii((char*)pwd);

    OesnBuiltInLoginModule* obj = ObjectWrap::Unwrap<OesnBuiltInLoginModule>(args.This());
    BuiltInLoginModule_add_account(obj->biimpl_, group, username, pwd);

    delete group;
    delete username;
    delete pwd;

    return args.This();
}

Handle<Value> OesnBuiltInLoginModule::AddPrivilege(const Arguments& args) {
    HandleScope scope;

    if (args.Length() != 3) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }
    if (args[0]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("no space named")));
        return scope.Close(Undefined());
    }
    if (args[1]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("no group")));
        return scope.Close(Undefined());
    }
    if (args[2]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("no spec")));
        return scope.Close(Undefined());
    }

    char *space;
    v8::Local<v8::String> s = args[0]->ToString();
    space = new char[s->Length() + 1];
    s->WriteAscii((char*)space);

    char *group;
    s = args[1]->ToString();
    group = new char[s->Length() + 1];
    s->WriteAscii((char*)group);

    char *spec;
    s = args[2]->ToString();
    spec = new char[s->Length() + 1];
    s->WriteAscii((char*)spec);

    OesnBuiltInLoginModule* obj = ObjectWrap::Unwrap<OesnBuiltInLoginModule>(args.This());

    OekMsg msg = 0;
    int rc = json_msg_create(&msg, spec, BuiltInLoginModule_get_arena(obj->biimpl_));
    if (rc) {
        printf("ejs spec: %i %s\n", rc, spec);
        ThrowException(Exception::TypeError(String::New("bad spec")));
        return scope.Close(Undefined());
    }

    BuiltInLoginModule_add_privilege(obj->biimpl_, space, group, msg);

    delete space;
    delete group;
    delete spec;

    return args.This();
}

