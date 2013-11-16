#ifndef OESNNET_H
#define OESNNET_H

#include <node.h>
#include <OeNet.h>
#include "OesnDispatcher.h"

class OesnNet : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> target);
  OeNet getImpl();

 protected:
  OesnNet(OesnDispatcher *);
  ~OesnNet();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> SetName(const v8::Arguments& args);
  OeNet impl_;
};

#endif

