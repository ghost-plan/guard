//
// Created by jamesfchen on 2022/3/27.
//

#ifndef ATTACK_ON_TITAN_SERVICEMANAGER_H
#define ATTACK_ON_TITAN_SERVICEMANAGER_H

#include <fbjni/fbjni.h>
#include <memory>
#include <string>
#include "BpBinder.h"
#include <log_ext.h>

using namespace facebook::jni;
namespace jamesfchen {
class ServiceManager : public facebook::jni::HybridClass<ServiceManager> {
 public:
  static constexpr auto
	  kJavaDescriptor = "Lcom/jamesfchen/binder/ServiceManager;";

  static void registerNatives();

  static local_ref<jhybriddata> initHybrid(alias_ref<jclass>);

  static local_ref<BpBinder::jhybridobject> getBinderProxy(alias_ref<jclass>);

 private:
  friend HybridBase;
};

}
#endif //ATTACK_ON_TITAN_SERVICEMANAGER_H
