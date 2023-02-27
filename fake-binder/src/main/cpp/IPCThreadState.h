//
// Created by jamesfchen on 2022/3/27.
//

#ifndef ATTACK_ON_TITAN_IPCTHREADSTATE_H
#define ATTACK_ON_TITAN_IPCTHREADSTATE_H

#include "data_struct_ext.h"
#include "ProcessState.h"
#include "jni/Parcel.h"

namespace jamesfchen {
class ProcessState;
class Parcel;
class IPCThreadState {
 public:
  static IPCThreadState *self();

  status_t transact(int32_t handle,
					uint32_t code, const Parcel &data,
					Parcel *reply, uint32_t flags);

  void joinThreadPool(bool isMain);

 private:
  IPCThreadState();

  ~IPCThreadState();

  status_t sendReply(const Parcel &reply, uint32_t flags);

  status_t waitForResponse(Parcel *reply,
						   status_t *acquireResult = NULL);

  status_t talkWithDriver(bool doReceive = true);

  status_t writeTransactionData(int32_t cmd,
								uint32_t binderFlags,
								int32_t handle,
								uint32_t code,
								const Parcel &data,
								status_t *statusBuffer);

  status_t getAndExecuteCommand();

  status_t executeCommand(int32_t command);

  static void freeBuffer(Parcel *parcel,
						 const uint8_t *data, size_t dataSize,
						 const binder_size_t *objects, size_t objectsSize,
						 void *cookie);

  ProcessState *mProcess{};
  Parcel mIn;
  Parcel mOut;
  status_t mLastError{};
  pid_t mCallingPid{};
  uid_t mCallingUid{};
  int32_t mStrictModePolicy{};
  int32_t mLastTransactionBinderFlags{};
};
}
#endif //ATTACK_ON_TITAN_IPCTHREADSTATE_H
