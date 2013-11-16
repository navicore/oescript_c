#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <iostream>
#include "OesnLoginModule.h"
#include <LoginModule.h>

using namespace v8;

OesnLoginModule::OesnLoginModule() {
};

OesnLoginModule::~OesnLoginModule() {
    //LoginModule_free(&impl_);
};

LoginModule OesnLoginModule::getImpl() {
    return impl_;
}

