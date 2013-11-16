#ifndef OESNTHRDISPATCHER_H
#define OESNTHRDISPATCHER_H

#include <node.h>
#include <Oe_Thread_Dispatcher.h>

class OesnThreadDispatcher : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> target);
  Oe_Thread_Dispatcher getImpl();

 protected:
  OesnThreadDispatcher(int);
  ~OesnThreadDispatcher();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Start(const v8::Arguments& args);
  Oe_Thread_Dispatcher impl_;
};

#endif

