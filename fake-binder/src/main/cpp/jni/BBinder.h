//
// Created by jamesfchen on 2022/3/27.
//

#ifndef ATTACK_ON_TITAN_BBINDER_H
#define ATTACK_ON_TITAN_BBINDER_H

#include "IBinder.h"

namespace jamesfchen {

class BBinder :
	public IBinder {
 public:
  BBinder();
  static constexpr auto kJavaDescriptor = "Lcom/jamesfchen/binder/Binder;";

  static void registerNatives();
  static void initHybrid(alias_ref<jhybridobject> o);
  virtual status_t transact(uint32_t code,
							const Parcel &data,
							Parcel *reply,
							uint32_t flags = 0);

 protected:
  virtual             ~BBinder();

  virtual status_t onTransact(uint32_t code,
							  const Parcel &data,
							  Parcel *reply,
							  uint32_t flags = 0);
};
};

#endif //ATTACK_ON_TITAN_BBINDER_H
