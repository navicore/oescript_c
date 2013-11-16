#ifndef OESN_BI_LM_H
#define OESN_BI_LM_H

#include <node.h>
#include <BuiltInLoginModule.h>
#include "OesnLoginModule.h"

class OesnBuiltInLoginModule : public OesnLoginModule {
 public:
  static void Init(v8::Handle<v8::Object> target);

 protected:
  OesnBuiltInLoginModule();
  ~OesnBuiltInLoginModule();

  BuiltInLoginModule biimpl_;

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> AddAccount(const v8::Arguments& args);
  static v8::Handle<v8::Value> AddPrivilege(const v8::Arguments& args);

};

#endif

