#include <string>
#include <fbjni/fbjni.h>
#include "OnLoad.h"
#include "BpBinder.h"
#include "ServiceManager.h"
#include "BBinder.h"
//#include <glog/logging.h>
namespace jamesfchen {

extern "C" JNIEXPORT jint
JNI_OnLoad(JavaVM *vm, void *reserved) {
  return facebook::jni::initialize(vm, [] {
//    gloginit::initialize();
//    FLAGS_minloglevel = 0;
	ServiceManager::registerNatives();
	BpBinder::registerNatives();
	BBinder::registerNatives();
	Parcel::registerNatives();
  });
}

}
