#ifndef ATTACK_ON_TITAN_STATIC_H
#define ATTACK_ON_TITAN_STATIC_H
#include "ProcessState.h"
namespace jamesfchen {

// For TextStream.cpp
//    extern Vector<int32_t> gTextBuffers;

// For ProcessState.cpp
//    extern Mutex gProcessMutex;
extern ProcessState *gProcess;

// For IServiceManager.cpp
//    extern Mutex gDefaultServiceManagerLock;
//    extern sp<IServiceManager> gDefaultServiceManager;
//    extern sp<IPermissionController> gPermissionController;

}   // namespace android
#endif //ATTACK_ON_TITAN_STATIC_H
