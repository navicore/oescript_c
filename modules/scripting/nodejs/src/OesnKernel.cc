#define BUILDING_NODE_EXTENSION
#include <v8.h>
#include <node.h>
#include <iostream>
#include "OesnKernel.h"
#include "OesnStore.h"
#include "OesnDispatcher.h"
#include "OesnThreadDispatcher.h"
#include <OEK.h>
#include <OEK_impl.h>

using namespace node;
using namespace v8;

#define REQ_FUN_ARG(I, VAR)                             \
if (args.Length() <= (I) || !args[I]->IsFunction())     \
return ThrowException(Exception::TypeError(             \
String::New("Argument " #I " must be a function")));    \
Local<Function> VAR = Local<Function>::Cast(args[I]);


OesnKernel::OesnKernel(OesnDispatcher *d, OesnThreadDispatcher *td, OesnStore *store) {
    impl_ =  OEK_default_factory_new(d->getImpl(), td->getImpl(), store->getImpl());
};
OesnKernel::~OesnKernel() {
    //JsonKernel_free(&impl_);
};

void OesnKernel::Init(Handle<Object> target) {

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("OesnKernel"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("OesnKernel"), constructor);
}

OEK OesnKernel::getImpl() {
    return impl_;
}

Handle<Value> OesnKernel::New(const Arguments& args) {
    HandleScope scope;

    if (args.Length() != 3) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    OesnDispatcher* d = ObjectWrap::Unwrap<OesnDispatcher>(args[0]->ToObject());
    OesnThreadDispatcher* td = ObjectWrap::Unwrap<OesnThreadDispatcher>(args[1]->ToObject());
    OesnStore* store = ObjectWrap::Unwrap<OesnStore>(args[2]->ToObject());

    OesnKernel* obj = new OesnKernel(d, td, store);
    obj->Wrap(args.This());

    return args.This();
}

