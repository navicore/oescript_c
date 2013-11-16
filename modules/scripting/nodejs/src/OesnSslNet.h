#ifndef OESNSSLNET_H
#define OESNSSLNET_H

#include <node.h>
#include <OeNet.h>
#include "OesnDispatcher.h"
#include "OesnNet.h"

//class OesnSslNet : public node::ObjectWrap {
class OesnSslNet : public OesnNet {
 public:
  static void Init(v8::Handle<v8::Object> target);
  //OeNet getImpl();

 protected:
  OesnSslNet(OesnDispatcher *, char *, char *);
  ~OesnSslNet();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  //OeNet impl_;
};

#endif

