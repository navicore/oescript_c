#ifndef OESNSTORE_H
#define OESNSTORE_H

#include <node.h>
#include <stdbool.h>
#include <OeStore.h>

class OesnStore : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> target);
  OeStore getImpl();

 protected:
  OesnStore(int, char *, bool);
  ~OesnStore();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  OeStore impl_;
};

#endif

