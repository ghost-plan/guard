//
// Created by jamesfchen on 2022/3/27.
//

#include <fbjni/fbjni.h>
#include <jni.h>

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    return facebook::jni::initialize(vm, [] {
//    gloginit::initialize();
//    FLAGS_minloglevel = 0;
    });
}
