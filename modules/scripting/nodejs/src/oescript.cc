#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "OesnDispatcher.h"
#include "OesnThreadDispatcher.h"
#include "OesnStore.h"
#include "OesnKernel.h"
#include "OesnJsonKernel.h"
#include "OesnNet.h"
#include "OesnSslNet.h"
#include "OesnServer.h"
#include "OesnLoginModule.h"
#include "OesnBuiltInLoginModule.h"
#include "OesnSigHandler.h"

using namespace v8;

void InitAll(Handle<Object> target) {
  OesnDispatcher::Init(target);
  OesnThreadDispatcher::Init(target);
  OesnStore::Init(target);
  OesnKernel::Init(target);
  OesnJsonKernel::Init(target);
  OesnNet::Init(target);
  OesnSslNet::Init(target);
  OesnServer::Init(target);
  OesnBuiltInLoginModule::Init(target);
  OesnSigHandler::Init(target);
}

NODE_MODULE(oescript, InitAll)

