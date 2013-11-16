#define BUILDING_NODE_EXTENSION
#include <v8.h>
#include <node.h>
#include <iostream>
#include "OesnJsonKernel.h"
#include "OesnKernel.h"
#include <OEK.h>
#include <OEK_impl.h>
#include <JsonKernel.h>

using namespace node;
using namespace v8;

#define REQ_FUN_ARG(I, VAR)                             \
if (args.Length() <= (I) || !args[I]->IsFunction())     \
return ThrowException(Exception::TypeError(             \
String::New("Argument " #I " must be a function")));    \
Local<Function> VAR = Local<Function>::Cast(args[I]);


OesnJsonKernel::OesnJsonKernel(OesnKernel *k) {
    impl_ =  JsonKernel_new(k->getImpl());
};
OesnJsonKernel::~OesnJsonKernel() {
    //JsonKernel_free(&impl_);
};

void OesnJsonKernel::Init(Handle<Object> target) {

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("OesnJsonKernel"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("exec"),
                                  FunctionTemplate::New(Exec)->GetFunction());

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("OesnJsonKernel"), constructor);
}

JsonKernel OesnJsonKernel::getImpl() {
    return impl_;
}

Handle<Value> OesnJsonKernel::New(const Arguments& args) {
    HandleScope scope;

    if (args.Length() != 1) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    OesnKernel* k = ObjectWrap::Unwrap<OesnKernel>(args[0]->ToObject());

    OesnJsonKernel* obj = new OesnJsonKernel(k);
    obj->Wrap(args.This());

    return args.This();
}

struct oek_baton_t {
    OesnJsonKernel *this_;
    Persistent<Function> cb;
    char *request;
    char *response;
};

void OesnJsonKernel::Exec_cb(char *json_res, void *arg)
{
    struct oek_baton_t *baton = (struct oek_baton_t *) arg;
    baton->response = oec_cpy_str(NULL, json_res);

    eio_nop (EIO_PRI_DEFAULT, OesnJsonKernel::EIO_AfterExec, baton);
}

int OesnJsonKernel::EIO_AfterExec(eio_req *req)
{
    HandleScope scope;
    struct oek_baton_t *baton = static_cast<struct oek_baton_t *>(req->data);
    ev_unref(EV_DEFAULT_UC);
    //uv_unref(EV_DEFAULT_UC);
    baton->this_->Unref();

    Local<Value> argv[1];

    argv[0] = String::New(baton->response);

    TryCatch try_catch;

    baton->cb->Call(Context::GetCurrent()->Global(), 1, argv);

    if (try_catch.HasCaught()) {
        FatalException(try_catch);
    }

    baton->cb.Dispose();

    delete baton->request;
    delete baton->response;
    delete baton;
    return 0;
}

Handle<Value> OesnJsonKernel::Exec(const Arguments& args) {

    HandleScope scope;

    OesnJsonKernel* obj = ObjectWrap::Unwrap<OesnJsonKernel>(args.This());
    JsonKernel impl = obj->getImpl();

    REQ_FUN_ARG(1, cb);

    char *json_cmd;
    if (args[0]->IsUndefined()) {
        ThrowException(Exception::TypeError(String::New("Missing JSON Arg")));
        return scope.Close(Undefined());
    } else {
        v8::Local<v8::String> s = args[0]->ToString();
        json_cmd = new char[s->Length() + 1];
        s->WriteAscii(json_cmd);
    }

    oek_baton_t *baton = new oek_baton_t();
    baton->this_ = obj;
    baton->cb = Persistent<Function>::New(cb);
    baton->request = json_cmd;

    obj->Ref();

    int rc = JsonKernel_exec(impl, json_cmd, OesnJsonKernel::Exec_cb, baton);
    if (rc) {
        char *emsg = "{'type':	'error', 'eid':	9, 'message':	'invalid oescript command'}";
        OesnJsonKernel::Exec_cb(emsg, baton);
    }

    ev_ref(EV_DEFAULT_UC);
    //uv_ref(EV_DEFAULT_UC);  //todo: use uvlib asap.  stuck at v8

    return Undefined();
}

