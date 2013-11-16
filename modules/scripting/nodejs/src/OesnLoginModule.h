#ifndef OESNLOGINMOD_H
#define OESNLOGINMOD_H

#include <node.h>
#include <LoginModule.h>

class OesnLoginModule : public node::ObjectWrap {
 public:
  LoginModule getImpl();
  static void Init(v8::Handle<v8::Object> target);

 protected:
  OesnLoginModule();
  ~OesnLoginModule();

  LoginModule impl_;
};

#endif

