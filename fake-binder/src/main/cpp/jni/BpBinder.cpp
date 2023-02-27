//
// Created by jamesfchen on 2022/3/27.
//

#include "BpBinder.h"
#include <log_ext.h>
#include "../IPCThreadState.h"

using namespace facebook::jni;
namespace jamesfchen {
void BpBinder::registerNatives() {
  registerHybrid({
					 makeNativeMethod("initHybrid", BpBinder::initHybrid),
					 makeNativeMethod("transactNative",
									  BpBinder::transactNative),
				 });
}

void BpBinder::initHybrid(facebook::jni::alias_ref<jhybridobject> o,
						  int handle) {
  // The arguments will be forwarded to the ctor, and the result
  // will be saved in mHybridPointer in the java object.
  setCxxInstance(o, handle);
}

bool BpBinder::transactNative(alias_ref<jhybridobject> o,
							  int code,
							  alias_ref<Parcel::javaobject> data,
							  alias_ref<Parcel::javaobject> reply,
							  int flags
) {
  if (data == nullptr || reply == nullptr) return JNI_FALSE;
  status_t err = o->cthis()->transact(code, data, reply, flags);
  LOG_D("fakebinder", "transact error:%d", err);
  const int NO_ERROR = 0;
  const int UNKNOWN_TRANSACTION = 1;
  if (err == NO_ERROR) {
	return JNI_TRUE;
  } else if (err == UNKNOWN_TRANSACTION) {
	return JNI_FALSE;
  }
  return JNI_FALSE;
}

status_t BpBinder::transact(int code, alias_ref<Parcel::javaobject> data,
							alias_ref<Parcel::javaobject> reply,
							int flags) {
  LOG_D("fakebinder", "BpBinder#transact");
  status_t
	  status = IPCThreadState::self()->transact(mHandle, code, *data->cthis(),
												reply->cthis(), flags);
  return status;
}
}