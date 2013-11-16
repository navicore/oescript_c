#ifndef OESN_SIGH_H
#define OESN_SIGHL_H

#include <node.h>
#include <SignalHandler.h>

#include "OesnKernel.h"
#include "OesnJsonKernel.h"
#include "OesnStore.h"
#include "OesnDispatcher.h"
#include "OesnThreadDispatcher.h"

struct oe_sighandler_baton_t;

class OesnSigHandler : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> target);
  SignalHandler getImpl();

 protected:
  OesnSigHandler(char *, char *, char *);
  ~OesnSigHandler();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Manage(const v8::Arguments& args);

  SignalHandler impl_;
};

#endif

