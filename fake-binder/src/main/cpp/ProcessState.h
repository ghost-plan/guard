//
// Created by jamesfchen on 2022/3/27.
//

#ifndef ATTACK_ON_TITAN_PROCESSSTATE_H
#define ATTACK_ON_TITAN_PROCESSSTATE_H

#include "data_struct_ext.h"
#include "IPCThreadState.h"
#include "jni/BpBinder.h"

namespace jamesfchen {
class IPCThreadState;
/**
 * ProcessState会启动一个线程不停的轮询驱动binder文件
 */
class ProcessState {
 public:
  /**
   *     virtual void onZygoteInit()
  {
	  sp<ProcessState> proc = ProcessState::self();
	  ALOGV("App process: starting thread pool.\n");
	  proc->startThreadPool();
  }
   当app进程被ZygoteInit fork出来时，开始初始化Runtime 、 ProcessState 、 Application
	   public static final void zygoteInit(int targetSdkVersion, String[] argv,
	  ClassLoader classLoader) throws Zygote.MethodAndArgsCaller {
			  。。。
			  RuntimeInit.redirectLogStreams();

			  RuntimeInit.commonInit();
			  ZygoteInit.nativeZygoteInit();
			  RuntimeInit.applicationInit(targetSdkVersion, argv, classLoader);
	  }
   */
  static ProcessState *self();//单例
//        static  sp<ProcessState>    initWithDriver(const char *driver);

  IBinder *getContextObject(const IBinder &caller);

//        void                setContextObject(const sp<IBinder>& object);
//        void                setContextObject(const sp<IBinder>& object,
//                                             const String16& name);
//        sp<IBinder>         getContextObject(const String16& name,
//                                             const sp<IBinder>& caller);
  IBinder *getStrongProxyForHandle(int32_t handle);

  typedef bool (*context_check_func)(const String16 &name,
									 const IBinder &caller,
									 void *userData);

  void startThreadPool();

  void spawnPooledThread(bool isMain);

 private:
  friend class IPCThreadState;

  ProcessState(const char *driver);

  ~ProcessState();

  ProcessState(const ProcessState &o);

  ProcessState &operator=(ProcessState *o);

  String8 makeBinderThreadName();

  String8 mDriverName;
  int mDriverFD;
  void *mVMStart;
  // Protects thread count variable below.
  pthread_mutex_t mThreadCountLock;
  pthread_cond_t mThreadCountDecrement;
  // Number of binder threads current executing a command.
  size_t mExecutingThreadsCount;
  // Maximum number for binder threads allowed for this process.
  size_t mMaxThreads;
  // Time when thread pool was emptied
  int64_t mStarvationStartTimeMs;

  bool mManagesContexts;
  context_check_func mBinderContextCheckFunc;
  void *mBinderContextUserData;
  bool mThreadPoolStarted;
  volatile int32_t mThreadPoolSeq;

};
}
#endif //ATTACK_ON_TITAN_PROCESSSTATE_H
