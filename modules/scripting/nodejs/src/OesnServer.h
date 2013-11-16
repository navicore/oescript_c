#ifndef OESNSERVER_H
#define OESNSERVER_H

#include <node.h>
#include <stdbool.h>
#include <OepServer.h>
#include "OesnDispatcher.h"
#include "OesnNet.h"
#include "OesnSslNet.h"
#include "OesnStore.h"

class OesnServer : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> target);

 protected:
  OesnServer(OesnDispatcher *, char *, OesnNet *, OesnStore *);
  ~OesnServer();
  OepServer getImpl();

  OepServer impl_;

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> AddLoginModule(const v8::Arguments& args);
  static v8::Handle<v8::Value> AddKernel(const v8::Arguments& args);
};

#endif

