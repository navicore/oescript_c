#ifndef OESN_KERNEL_H
#define OESN_KERNEL_H

#include <node.h>
#include <OEK.h>
#include "OesnStore.h"
#include "OesnDispatcher.h"
#include "OesnThreadDispatcher.h"

struct oek_baton_t;

class OesnKernel : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> target);
  OEK getImpl();

 protected:
  OesnKernel(OesnDispatcher *, OesnThreadDispatcher *, OesnStore *);
  ~OesnKernel();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);

  OEK impl_;
};

#endif

