#include <cstdint>
#include <log_ext.h>
#include "BBinder.h"
namespace jamesfchen {
void BBinder::registerNatives() {
  registerHybrid({
					 makeNativeMethod("initHybrid", BBinder::initHybrid),
//					 makeNativeMethod("onTransact",
//									  BBinder::onTransact),
				 });
}
void BBinder::initHybrid(facebook::jni::alias_ref<jhybridobject> o) {
  // The arguments will be forwarded to the ctor, and the result
  // will be saved in mHybridPointer in the java object.
  setCxxInstance(o);
}
BBinder::BBinder() {
//        LOG_D("fakebinder", "BBinder#构造器");
}

BBinder::~BBinder() {
//        LOG_D("fakebinder", "BBinder#析构器");

}

status_t BBinder::transact(uint32_t code,
						   const Parcel &data,
						   Parcel *reply,
						   uint32_t flags) {
  LOG_D("fakebinder", "BBinder#transact");
  return 0;
}

status_t BBinder::onTransact(uint32_t code,
							 const Parcel &data,
							 Parcel *reply,
							 uint32_t flags) {
  LOG_D("fakebinder", "BBinder#onTransact");
  return 0;
}
}

