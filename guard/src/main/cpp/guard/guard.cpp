//
// Created by guard.jamesf on 12/19/19.
//
#include <string>
#include <jni.h>
#include <cstring>
#include <cinttypes>
#include <pthread.h>
#include <android/log.h>
#include <cassert>
#include <ctime>
#include <dlfcn.h>
#include <search.h>
#include "so_protector.h"
#include "event.h"
#include "log_ext.h"

#define CLASS_PATH "com/jamesfchen/guard/TestGuardActivity"
#define   NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#define   LOG_TAG    "cjf_defense_jni"
const char *RELEASE_SIGN = "E5:1B:40:36:1E:5E:E0:FF:82:54:64:65:06:B2:0F:93:6E:D4:17:77";
static jboolean auth = JNI_FALSE;
//import class
jclass SignatureClass;
jclass PackageInfoClass;
jclass ContextClass;
jclass PackageManagerClass;
jclass IPackageManagerClass;
jclass ApplicationInfoClass;
jclass ServiceManagerClass;
jclass IPackageManager$StubClass;
jclass UtilClass;
jclass ProxyClass;

void import_class(JNIEnv *env) {
    SignatureClass = env->FindClass("android/content/pm/Signature");
    PackageInfoClass = env->FindClass("android/content/pm/PackageInfo");
    ContextClass = env->FindClass("android/content/Context");
    PackageManagerClass = env->FindClass("android/content/pm/PackageManager");
    IPackageManagerClass = env->FindClass("android/content/pm/IPackageManager");
    IPackageManager$StubClass = env->FindClass("android/content/pm/IPackageManager$Stub");
    ApplicationInfoClass = env->FindClass("android/content/pm/ApplicationInfo");
    ServiceManagerClass = env->FindClass("android/os/ServiceManager");
    UtilClass = env->FindClass("com/jamesfchen/common/ToolBox");
    ProxyClass = env->FindClass("java/lang/reflect/Proxy");
}

void clear_class(JNIEnv *env) {
    env->DeleteLocalRef(ContextClass);
    env->DeleteLocalRef(SignatureClass);
    env->DeleteLocalRef(PackageManagerClass);
    env->DeleteLocalRef(IPackageManagerClass);
    env->DeleteLocalRef(IPackageManager$StubClass);
    env->DeleteLocalRef(PackageInfoClass);
    env->DeleteLocalRef(ApplicationInfoClass);
    env->DeleteLocalRef(ServiceManagerClass);
    env->DeleteLocalRef(UtilClass);
    env->DeleteLocalRef(ProxyClass);
}

int i = 0;
extern "C" JNIEXPORT  JNICALL
jstring Java_com_jamesfchen_guard_TestGuardActivity_stringFromJNI(JNIEnv *env,
                                                                  jobject caller /* this */) {
    std::string hello = "Hello from C++ ";
    hello.append<int>(i, 0x2E);
    ++i;

    return env->NewStringUTF(hello.c_str());
}

jbyteArray get_sign_byteArray(JNIEnv *env, jobject packageInfoObject) {
    if (packageInfoObject == nullptr) {
        LOG_E(LOG_TAG, "packageinfo object is null");
        return env->NewByteArray(0);
    }
    jfieldID signaturefieldID = env->GetFieldID(PackageInfoClass, "signatures",
                                                "[Landroid/content/pm/Signature;");
    jmethodID toByteArrayMethodid = env->GetMethodID(SignatureClass, "toByteArray", "()[B");
    jobjectArray signatureArray = (jobjectArray) env->GetObjectField(packageInfoObject,
                                                                     signaturefieldID);
    jobject signatureObject = env->GetObjectArrayElement(signatureArray, 0);
    return (jbyteArray) env->CallObjectMethod(signatureObject,
                                              toByteArrayMethodid);
}

jstring bytes_to_hexstring(JNIEnv *env, jbyteArray datas) {
    jmethodID methodID = env->GetStaticMethodID(UtilClass, "sha1ToHexString",
                                                "([B)Ljava/lang/String;");
    return (jstring) env->CallStaticObjectMethod(UtilClass, methodID, datas);
}


extern "C" JNIEXPORT JNICALL
jstring get_sign_v2(JNIEnv *env, jobject caller /* this */, jobject contextObject) {
//    mPM.getPackageInfo(packageName, flags, userId);
    import_class(env);

    jmethodID getPackageNameId = env->GetMethodID(ContextClass, "getPackageName",
                                                  "()Ljava/lang/String;");
    jmethodID getPackageManagerId = env->GetMethodID(ContextClass, "getPackageManager",
                                                     "()Landroid/content/pm/PackageManager;");
    jmethodID getApplicationInfoId = env->GetMethodID(PackageManagerClass, "getApplicationInfo",
                                                      "(Ljava/lang/String;I)Landroid/content/pm/ApplicationInfo;");
    jfieldID useridId = (jfieldID) env->GetFieldID(ApplicationInfoClass, "uid", "I");


    jstring packNameString = (jstring) env->CallObjectMethod(contextObject, getPackageNameId);
    jobject packageManagerObject = env->CallObjectMethod(contextObject, getPackageManagerId);
    jobject applicationInfoObject = env->CallObjectMethod(packageManagerObject,
                                                          getApplicationInfoId, packNameString,
                                                          0x00000080);


    jint userid = (jint) env->GetIntField(applicationInfoObject, useridId);
    const char *pkgName = env->GetStringUTFChars(packNameString, 0);
    LOG_E(LOG_TAG, "package name:%s uid:%d", pkgName, userid);


    jmethodID getServiceId = env->GetStaticMethodID(ServiceManagerClass, "getService",
                                                    "(Ljava/lang/String;)Landroid/os/IBinder;");
    jmethodID asInterfaceId = env->GetStaticMethodID(IPackageManager$StubClass, "asInterface",
                                                     "(Landroid/os/IBinder;)Landroid/content/pm/IPackageManager;");

    jstring packageStr = env->NewStringUTF("package");

    jobject iBinderObj = env->CallStaticObjectMethod(ServiceManagerClass, getServiceId, packageStr);
    if (iBinderObj == nullptr) {
        LOG_E(LOG_TAG, "iBinderObj  is null");
    }
    jobject iPackageManagerObj = env->CallStaticObjectMethod(IPackageManager$StubClass,
                                                             asInterfaceId,
                                                             iBinderObj);
    if (iPackageManagerObj == nullptr) {
        LOG_E(LOG_TAG, "iPackageManagerObj  is null");
    }
    jmethodID getPackageInfoId = env->GetMethodID(IPackageManagerClass, "getPackageInfo",
                                                  "(Ljava/lang/String;II)Landroid/content/pm/PackageInfo;");

    jobject packageInfoObject = env->CallObjectMethod(iPackageManagerObj, getPackageInfoId,
                                                      packNameString, 64, 0);

    jbyteArray signByte = get_sign_byteArray(env, packageInfoObject);
    return bytes_to_hexstring(env, signByte);

}

extern "C" JNIEXPORT JNICALL
jstring get_sign(JNIEnv *env, jobject caller /* this */, jobject contextObject) {
    import_class(env);
    jmethodID getPackageManagerId = env->GetMethodID(ContextClass, "getPackageManager",
                                                     "()Landroid/content/pm/PackageManager;");
    jmethodID getPackageNameId = env->GetMethodID(ContextClass, "getPackageName",
                                                  "()Ljava/lang/String;");
    jmethodID getPackageInfoId = env->GetMethodID(PackageManagerClass, "getPackageInfo",
                                                  "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");

    jobject packageManagerObject = env->CallObjectMethod(contextObject, getPackageManagerId);
    jstring packNameString = (jstring) env->CallObjectMethod(contextObject, getPackageNameId);
    jobject packageInfoObject = env->CallObjectMethod(packageManagerObject, getPackageInfoId,
                                                      packNameString, 64);
    jbyteArray signByte = get_sign_byteArray(env, packageInfoObject);
    jstring hexstirng = bytes_to_hexstring(env, signByte);
    clear_class(env);
    return hexstirng;
}
/**
 * 从android7.0开始限制dlopen调用
 */
typedef  jboolean (*Entry)();
extern "C" JNIEXPORT JNICALL
jboolean dynamic_loader(JNIEnv *env, jobject caller, jstring path) {
    void *handle = dlopen(env->GetStringUTFChars(path, 0), RTLD_LAZY | RTLD_LOCAL);
    if (!handle) {
        LOG_I(LOG_TAG, "handle 为空");
        return false;
    }
    //dlsym()函数所做的就是解析so文件的.dynamic 通过符号表来获得函数的地址
    Entry entry = (Entry)dlsym(handle, "entry");

    if (!entry) {
        LOG_I(LOG_TAG, "sym 为空");
        dlclose(handle);
        return false;
    }
    entry();
    return true;
}

extern "C" JNIEXPORT JNICALL
jboolean verify(JNIEnv *env, jobject caller, jobject contextObject) {
    import_class(env);
    LOG_I(LOG_TAG, "jni 第一种获取签名的方法: %s",
          env->GetStringUTFChars(get_sign(env, caller, contextObject), 0));
    LOG_I(LOG_TAG, "jni 第二种获取签名的方法：%s",
          env->GetStringUTFChars(get_sign_v2(env, caller, contextObject), 0));
    jmethodID isProxyClassMethodId = env->GetStaticMethodID(ProxyClass, "isProxyClass",
                                                            "(Ljava/lang/Class;)Z");
    jmethodID getServiceId = env->GetStaticMethodID(ServiceManagerClass, "getService",
                                                    "(Ljava/lang/String;)Landroid/os/IBinder;");
    jmethodID asInterfaceId = env->GetStaticMethodID(IPackageManager$StubClass, "asInterface",
                                                     "(Landroid/os/IBinder;)Landroid/content/pm/IPackageManager;");

    jstring packageStr = env->NewStringUTF("package");

    jobject iBinderObj = env->CallStaticObjectMethod(ServiceManagerClass, getServiceId, packageStr);
    if (iBinderObj == nullptr) {
        LOG_E(LOG_TAG, "iBinderObj  is null");
    }
    jobject iPackageManagerObj = env->CallStaticObjectMethod(IPackageManager$StubClass,
                                                             asInterfaceId,
                                                             iBinderObj);
    if (iPackageManagerObj == nullptr) {
        LOG_E(LOG_TAG, "iPackageManagerObj  is null");
    }
    jboolean isProxyClass = env->CallStaticBooleanMethod(ProxyClass, isProxyClassMethodId,
                                                         env->GetObjectClass(iPackageManagerObj));
    clear_class(env);
    if (isProxyClass) {
        LOG_E(LOG_TAG, "PackageManger使用了动态代理，被攻击了！！！！");
        return JNI_FALSE;

    } else {
        LOG_E(LOG_TAG, "PackageManger来自系统提供，没有被攻击");
        return JNI_FALSE;
    }
}

//extern "C"
JNIEXPORT JNICALL
jboolean entry(JNIEnv *env, jobject caller, jobject contextObject) {
    so_protector::entry();
    jamesfchen_event::entry();

    return JNI_TRUE;

}
JNIEXPORT JNICALL
jboolean test_event(JNIEnv *env, jobject caller, jobject contextObject) {
    jamesfchen_event::TestEvent();
    return JNI_TRUE;
}
/*
* JNI registration.
*/
static JNINativeMethod gMethods[] = {
        /* name, signature, funcPtr */
//        {"setup",         "(ZI)Z",                                                   (void *) setup},
//        {"replaceMethod", "(Ljava/lang/reflect/Method;Ljava/lang/reflect/Method;)V", (void *) replaceMethod},
//        {"setFieldFlag",  "(Ljava/lang/reflect/Field;)V",                            (void *) setFieldFlag},
//        {"getSign",   "(Landroid/content/Context;)Ljava/lang/String;", (void *) get_sign},
//        {"getSignv2", "(Landroid/content/Context;)Ljava/lang/String;", (void *) get_sign_v2},
        {"testEvent",        "(Landroid/content/Context;)Z", (void *) test_event},
        {"verify",        "(Landroid/content/Context;)Z", (void *) verify},
        {"dynamicLoader", "(Ljava/lang/String;)Z",        (void *) dynamic_loader},
        {"main",        "(Landroid/content/Context;)Z", (void *) entry}
};

static int registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *gMethods,
                                 int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }

    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static int registerNatives(JNIEnv *env) {
    return registerNativeMethods(env, CLASS_PATH, gMethods, NELEM(gMethods));
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

    LOG_I(LOG_TAG, "JNI_ONLOAD");

    JNIEnv *env = NULL;
    jint result = -1;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOG_I(LOG_TAG, "JNI_ONLOAD:%s", "failed");
        return JNI_ERR;
    }
    assert(env != NULL);
    if (!registerNatives(env)) {//注册
        LOG_I(LOG_TAG, "registerNatives -1");
        return -1;
    }
    result = JNI_VERSION_1_6;

    return result;
}


JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {
    __android_log_print(ANDROID_LOG_DEBUG, "cjf", "JNI_OnUnload");
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return;
    }

}