//
// Created by jamesfchen on 2022/3/27.
//

#include "ServiceManager.h"

namespace jamesfchen {
void ServiceManager::registerNatives() {
  registerHybrid({
					 makeNativeMethod("initHybrid", ServiceManager::initHybrid),
					 makeNativeMethod("getBinderProxy",
									  ServiceManager::getBinderProxy),
				 });
}

facebook::jni::local_ref<ServiceManager::jhybriddata>
ServiceManager::initHybrid(facebook::jni::alias_ref<jclass>) {
  return makeCxxInstance();
}
local_ref<BpBinder::jhybridobject>
ServiceManager::getBinderProxy(facebook::jni::alias_ref<jclass>) {
//        jclass BinderProxyClass = env->FindClass("com/jamesfchen/binder/BinderProxy");
//        jmethodID constructorMethodId = env->GetMethodID(BinderProxyClass, "<init>", "(J)V");
  //当handle == 0 描述
  // Special case for context manager...
  // The context manager is the only object for which we create
  // a BpBinder proxy without already holding a reference.
  // Perform a dummy transaction to ensure the context manager
  // is registered before we create the first local reference
  // to it (which will occur when creating the BpBinder).
  // If a local reference is created for the BpBinder when the
  // context manager is not present, the driver will fail to
  // provide a reference to the context manager, but the
  // driver API does not return status.
  //
  // Note that this is not race-free if the context manager
  // dies while this code runs.
  //
  // TODO: add a driver API to wait for context manager, or
  // stop special casing handle 0 for context manager and add
  // a driver API to get a handle to the context manager with
  // proper reference counting.
//        IBinder *b = new BpBinder(0);
//        jobject binderProxyObject = env->NewObject(BinderProxyClass, constructorMethodId, (jlong) b);
//        LOG_D("fakebinder", "获取native BinderProxy[%d]", (jlong) b);
//        if (binderProxyObject != NULL) {
//        }
//        return binderProxyObject;
		//参数传给java对象
		//    return BpBinder:newObjectJavaArgs();
  		//参数传给cpp对象
  return BpBinder::newObjectCxxArgs(0);
}
}