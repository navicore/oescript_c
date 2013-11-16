#ifndef OESNDISPATCHER_H
#define OESNDISPATCHER_H

#include <node.h>
#include <Oed_Dispatcher.h>

class OesnDispatcher : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> target);
  Oed_Dispatcher getImpl();

 protected:
  OesnDispatcher(int, char *);
  ~OesnDispatcher();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Start(const v8::Arguments& args);
  Oed_Dispatcher impl_;
};

#endif

