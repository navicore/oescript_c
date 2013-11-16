#ifndef OESN_JSON_KERNEL_H
#define OESN_JSON_KERNEL_H

#include <node.h>
#include <JsonKernel.h>
#include "OesnKernel.h"

struct oek_baton_t;

class OesnJsonKernel : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> target);
  JsonKernel getImpl();

 protected:
  OesnJsonKernel(OesnKernel *);
  ~OesnJsonKernel();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Exec(const v8::Arguments& args);

  static void Exec_cb(char *, void *);
  static int EIO_AfterExec(eio_req *req);

  JsonKernel impl_;
};

#endif

