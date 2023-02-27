//
// Created by jamesfchen on 2022/3/27.
//

#ifndef ATTACK_ON_TITAN_IBINDER_H
#define ATTACK_ON_TITAN_IBINDER_H

#include "Parcel.h"
#include "../data_struct_ext.h"
#include <fbjni//fbjni.h>

using namespace facebook::jni;

namespace jamesfchen {
class Parcel;

class IBinder : public facebook::jni::HybridClass<IBinder> {
 public:
  static constexpr auto kJavaDescriptor = "Lcom/jamesfchen/binder/IBinder;";

//        virtual status_t
//        transact(int code, Parcel *data,
//                 Parcel *reply,
//                 int flags = 0) = 0;
};
}

#endif //ATTACK_ON_TITAN_IBINDER_H
