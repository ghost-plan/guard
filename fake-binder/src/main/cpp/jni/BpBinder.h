//
// Created by jamesfchen on 2022/3/27.
//

#ifndef ATTACK_ON_TITAN_BPBINDER_H
#define ATTACK_ON_TITAN_BPBINDER_H

#include <fbjni/fbjni.h>
#include <memory>
#include <string>
#include "IBinder.h"

using namespace facebook::jni;
namespace jamesfchen {

class BpBinder : public facebook::jni::HybridClass<BpBinder, IBinder> {
 public:
  BpBinder(const int32_t handle) : mHandle(handle) {};

  static constexpr auto kJavaDescriptor = "Lcom/jamesfchen/binder/BinderProxy;";

  static void registerNatives();

  static void initHybrid(alias_ref<jhybridobject> o, int handle);

  static bool
  transactNative(alias_ref<jhybridobject> o,
				 int code,
				 alias_ref<Parcel::javaobject> data,
				 alias_ref<Parcel::javaobject> reply,
				 int flags = 0);

  virtual status_t transact(int code, alias_ref<Parcel::javaobject> data,
							alias_ref<Parcel::javaobject> reply,
							int flags = 0);

 private:
  friend HybridBase;
  const int32_t mHandle;//real binder
};

}
#endif //ATTACK_ON_TITAN_BPBINDER_H
