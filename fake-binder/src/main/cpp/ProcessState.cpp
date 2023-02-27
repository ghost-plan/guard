#include <cstdio>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "Static.h"
#include <mutex>
#include <thread>
#include <fcntl.h>
#include <linux/android/binder.h>
#include <jni.h>
#include "ProcessState.h"

#define BINDER_VM_SIZE ((1 * 1024 * 1024) - sysconf(_SC_PAGE_SIZE) * 2)
#define DEFAULT_MAX_BINDER_THREADS 15
namespace jamesfchen {
class Thread {
 public:
  void run(const char *binderName) {

  }
};

class PoolThread : public Thread {
 public:
  explicit PoolThread(bool isMain)
	  : mIsMain(isMain) {
  }

 protected:
  //进入线程轮询
  virtual bool threadLoop() {
	IPCThreadState::self()->joinThreadPool(mIsMain);
	return false;
  }

  const bool mIsMain;
};

ProcessState *ProcessState::self() {
//    Mutex::Autolock _l(gProcessMutex);
  if (gProcess != NULL) {
	return gProcess;
  }
  gProcess = new ProcessState("/dev/binder");
  return gProcess;
}

IBinder *ProcessState::getContextObject(const IBinder &caller) {
  return getStrongProxyForHandle(0);
}

/**
 * 初始化ProcessState之后AndroidRuntime会调用startThreadPool
 * 创建binder主线程，线程名为binder_1，该线程永不停止
 */
void ProcessState::startThreadPool() {
//        AutoMutex _l(mLock);
  if (!mThreadPoolStarted) {
	mThreadPoolStarted = true;
	spawnPooledThread(true);
  }
}

IBinder *ProcessState::getStrongProxyForHandle(int32_t handle) {
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
  IBinder *b = nullptr;
  if (handle == 0) {
	b = new jamesfchen::BpBinder(handle);
  }
  return b;
}

String8 ProcessState::makeBinderThreadName() {
//        int32_t s = android_atomic_add(1, &mThreadPoolSeq);
  pid_t pid = getpid();
  String8 name;
//        name.appendFormat("Binder:%d_%X", pid, s);
  return name;
}

void ProcessState::spawnPooledThread(bool isMain) {
  if (mThreadPoolStarted) {
	String8 name = makeBinderThreadName();
//            ALOGV("Spawning new pooled thread, name=%s\n", name.string());
	Thread *t = new PoolThread(isMain);
	t->run(name.string());
  }
}

static int open_driver(const char *driver) {
  int fd = open(driver, O_RDWR | O_CLOEXEC);
  if (fd >= 0) {
	int vers = 0;
	status_t result = ioctl(fd, BINDER_VERSION, &vers);
	if (result == -1) {
//                ALOGE("Binder ioctl to obtain version failed: %s", strerror(errno));
	  close(fd);
	  fd = -1;
	}
	if (result != 0 || vers != BINDER_CURRENT_PROTOCOL_VERSION) {
//                ALOGE("Binder driver protocol(%d) does not match user space protocol(%d)! ioctl() return value: %d",
//                      vers, BINDER_CURRENT_PROTOCOL_VERSION, result);
	  close(fd);
	  fd = -1;
	}
	size_t maxThreads = DEFAULT_MAX_BINDER_THREADS;
	result = ioctl(fd, BINDER_SET_MAX_THREADS, &maxThreads);
	if (result == -1) {
//                ALOGE("Binder ioctl to set max threads failed: %s", strerror(errno));
	}
  } else {
//            ALOGW("Opening '%s' failed: %s\n", driver, strerror(errno));
  }
  return fd;
}

ProcessState::ProcessState(const char *driver)
	: mDriverName(String8(driver)),
	  mDriverFD(open_driver(driver)),
	  mVMStart(MAP_FAILED),
	  mThreadCountLock(PTHREAD_MUTEX_INITIALIZER),
	  mThreadCountDecrement(PTHREAD_COND_INITIALIZER),
	  mExecutingThreadsCount(0),
	  mMaxThreads(DEFAULT_MAX_BINDER_THREADS),
	  mStarvationStartTimeMs(0),
	  mManagesContexts(false),
	  mBinderContextCheckFunc(NULL),
	  mBinderContextUserData(NULL),
	  mThreadPoolStarted(false),
	  mThreadPoolSeq(1) {
  if (mDriverFD >= 0) {
	/**
	 * 映射文件到进程的地址空间，直接读写文件将
	 */
	// mmap the binder, providing a chunk of virtual address space to receive transactions.
	mVMStart = mmap(0,
					BINDER_VM_SIZE,
					PROT_READ,
					MAP_PRIVATE | MAP_NORESERVE,
					mDriverFD,
					0);
	if (mVMStart == MAP_FAILED) {
	  // *sigh*
//                ALOGE("Using /dev/binder failed: unable to mmap transaction memory.\n");
	  close(mDriverFD);
	  mDriverFD = -1;
//                mDriverName.clear();
	}
  }

//        LOG_ALWAYS_FATAL_IF(mDriverFD < 0, "Binder driver could not be opened.  Terminating.");
}

ProcessState::~ProcessState() {
  if (mDriverFD >= 0) {
	if (mVMStart != MAP_FAILED) {
	  munmap(mVMStart, BINDER_VM_SIZE);
	}
	close(mDriverFD);
  }
  mDriverFD = -1;
}

}