#include "IPCThreadState.h"
#include "jni/BBinder.h"
#include <linux/android/binder.h>
#include <fcntl.h>
#include <cstdlib>
#include <sys/mman.h>
#include <fstream>
#include <unistd.h>

#define NO_ERROR 0
namespace jamesfchen {

    IPCThreadState *IPCThreadState::self() {
        return new IPCThreadState;
    }

    IPCThreadState::IPCThreadState() {

    }

    IPCThreadState::~IPCThreadState() {

    }
    /**
     * binder其他线程，调用joinThreadPool会将当前线程直接加入到binder队列，例如mediaserver、servicemanager主线程都是binder线程，system server主线程不是binder线程
     * @param isMain
     */
    void IPCThreadState::joinThreadPool(bool isMain){
//        LOG_THREADPOOL("**** THREAD %p (PID %d) IS JOINING THE THREAD POOL\n", (void*)pthread_self(), getpid());

        mOut.writeInt32(isMain ? BC_ENTER_LOOPER : BC_REGISTER_LOOPER);

        status_t result;
        do {
//            processPendingDerefs();
            // now get the next command to be processed, waiting if necessary
            result = getAndExecuteCommand();

//            if (result < NO_ERROR && result != TIMED_OUT && result != -ECONNREFUSED && result != -EBADF) {
//                ALOGE("getAndExecuteCommand(fd=%d) returned unexpected error %d, aborting",mProcess->mDriverFD, result);
//                abort();
//            }

            // Let this thread exit the thread pool if it is no longer
            // needed and it is not the main process thread.
//            if(result == TIMED_OUT && !isMain) {
//                break;
//            }
        } while (result != -ECONNREFUSED && result != -EBADF);

//        LOG_THREADPOOL("**** THREAD %p (PID %d) IS LEAVING THE THREAD POOL err=%d\n",
//                       (void*)pthread_self(), getpid(), result);

        mOut.writeInt32(BC_EXIT_LOOPER);
//        talkWithDriver(false);
//        talkWithDriver();
    }
    /**
    *copy data from user space to  kernel space（talkWithDriver-->ioctl系統調用）/dev/binder
    */
    status_t
    IPCThreadState::transact(int32_t handle, uint32_t code, const Parcel &data, Parcel *reply,
                             uint32_t flags) {

        status_t err = data.errorCheck();
//
        flags |= TF_ACCEPT_FDS;
        if (err == NO_ERROR) {
//            LOG_ONEWAY(">>>> SEND from pid %d uid %d %s", getpid(), getuid(),
//                       (flags & TF_ONE_WAY) == 0 ? "READ REPLY" : "ONE WAY");
            err = writeTransactionData(BC_TRANSACTION, flags, handle, code, data, NULL);
        }
        if (err != NO_ERROR) {
            if (reply) reply->setError(err);
            return (mLastError = err);
        }
        if ((flags & TF_ONE_WAY) == 0) {
            if (reply) {
                err = waitForResponse(reply);
            } else {
                Parcel fakeReply;
                err = waitForResponse(&fakeReply);
            }
        } else {
            err = waitForResponse(NULL, NULL);
        }

        return err;
    }

    /**
     * 将BpBinder的Parcel data对象数据写入到IPCThreadState的Parcel mOut
     * @param cmd
     * @param binderFlags
     * @param handle
     * @param code
     * @param data
     * @param statusBuffer
     * @return
     */
    status_t IPCThreadState::writeTransactionData(int32_t cmd, uint32_t binderFlags, int32_t handle,
                                                  uint32_t code, const Parcel &data,
                                                  status_t *statusBuffer) {
        binder_transaction_data tr;

        tr.target.ptr = 0; /* Don't pass uninitialized stack data to a remote process */
        tr.target.handle = handle;
        tr.code = code;
        tr.flags = binderFlags;
        tr.cookie = 0;
        tr.sender_pid = 0;
        tr.sender_euid = 0;

        const status_t err = data.errorCheck();
        if (err == NO_ERROR) {
            tr.data_size = data.ipcDataSize();
            tr.data.ptr.buffer = (binder_uintptr_t) data.ipcData();
            tr.offsets_size = data.ipcObjectsCount() * sizeof(binder_size_t);
            tr.data.ptr.offsets = (binder_uintptr_t) data.ipcObjects();
        } else if (statusBuffer) {
            tr.flags |= TF_STATUS_CODE;
            *statusBuffer = err;
            tr.data_size = sizeof(status_t);
            tr.data.ptr.buffer = reinterpret_cast<uintptr_t>(statusBuffer);
            tr.offsets_size = 0;
            tr.data.ptr.offsets = 0;
        } else {
            return (mLastError = err);
        }

        mOut.writeInt32(cmd);
        mOut.write(&tr, sizeof(tr));

        return NO_ERROR;

    }

    /**
     * 将IPCThreadState的mOUt数据写入到bwr结构体，再通过kernel中ioctl系统调用将数据写会mIn，从而完成跨进程通讯
     * @param doReceive
     * @return
     */
    status_t IPCThreadState::talkWithDriver(bool doReceive) {
        if (mProcess->mDriverFD <= 0) {
            return -EBADF;
        }

        binder_write_read bwr;

        // Is the read buffer empty?
        const bool needRead = mIn.dataPosition() >= mIn.dataSize();

        // We don't want to write anything if we are still reading
        // from data left in the input buffer and the caller
        // has requested to read the next data.
        const size_t outAvail = (!doReceive || needRead) ? mOut.dataSize() : 0;

        bwr.write_size = outAvail;
        bwr.write_buffer = (uintptr_t) mOut.data();

        // This is what we'll read.
        if (doReceive && needRead) {
            bwr.read_size = mIn.dataCapacity();
            bwr.read_buffer = (uintptr_t) mIn.data();
        } else {
            bwr.read_size = 0;
            bwr.read_buffer = 0;
        }

//        IF_LOG_COMMANDS()
//        {
//            TextOutput::Bundle _b(alog);
//            if (outAvail != 0) {
//                alog << "Sending commands to driver: " << indent;
//                const void *cmds = (const void *) bwr.write_buffer;
//                const void *end = ((const uint8_t *) cmds) + bwr.write_size;
//                alog << HexDump(cmds, bwr.write_size) << endl;
//                while (cmds < end) cmds = printCommand(alog, cmds);
//                alog << dedent;
//            }
//            alog << "Size of receive buffer: " << bwr.read_size
//                 << ", needRead: " << needRead << ", doReceive: " << doReceive << endl;
//        }

        // Return immediately if there is nothing to do.
        if ((bwr.write_size == 0) && (bwr.read_size == 0)) return NO_ERROR;

        bwr.write_consumed = 0;
        bwr.read_consumed = 0;
        status_t err;
        do {
//            IF_LOG_COMMANDS()
//            {
//                alog << "About to read/write, write size = " << mOut.dataSize() << endl;
//            }
#if defined(__ANDROID__)
            if (ioctl(mProcess->mDriverFD, BINDER_WRITE_READ, &bwr) >= 0)
                err = NO_ERROR;
            else
                err = -errno;
#else
            err = INVALID_OPERATION;
#endif
            if (mProcess->mDriverFD <= 0) {
                err = -EBADF;
            }
//            IF_LOG_COMMANDS()
//            {
//                alog << "Finished read/write, write size = " << mOut.dataSize() << endl;
//            }
        } while (err == -EINTR);

//        IF_LOG_COMMANDS()
//        {
//            alog << "Our err: " << (void *) (intptr_t) err << ", write consumed: "
//                 << bwr.write_consumed << " (of " << mOut.dataSize()
//                 << "), read consumed: " << bwr.read_consumed << endl;
//        }

        if (err >= NO_ERROR) {
            if (bwr.write_consumed > 0) {
                if (bwr.write_consumed < mOut.dataSize())
                    mOut.remove(0, bwr.write_consumed);
                else
                    mOut.setDataSize(0);
            }
            if (bwr.read_consumed > 0) {
                mIn.setDataSize(bwr.read_consumed);
                mIn.setDataPosition(0);
            }
//            IF_LOG_COMMANDS()
//            {
//                TextOutput::Bundle _b(alog);
//                alog << "Remaining data size: " << mOut.dataSize() << endl;
//                alog << "Received commands from driver: " << indent;
//                const void *cmds = mIn.data();
//                const void *end = mIn.data() + mIn.dataSize();
//                alog << HexDump(cmds, mIn.dataSize()) << endl;
//                while (cmds < end) cmds = printReturnCommand(alog, cmds);
//                alog << dedent;
//            }
            return NO_ERROR;
        }

        return err;
    }

    /**
     * 开启一个轮询，将数据从mOut写入到
     * @param reply
     * @param acquireResult
     * @return
     */
    status_t IPCThreadState::waitForResponse(Parcel *reply, status_t *acquireResult) {
        uint32_t cmd;
        int32_t err;

        while (true) {
            if ((err = talkWithDriver()) < NO_ERROR) break;
            err = mIn.errorCheck();
            if (err < NO_ERROR) break;
            if (mIn.dataAvail() == 0) continue;

            cmd = (uint32_t) mIn.readInt32();

//            IF_LOG_COMMANDS() {
//                alog << "Processing waitForResponse Command: "
//                     << getReturnString(cmd) << endl;
//            }

            switch (cmd) {
                case BR_TRANSACTION_COMPLETE:
                    if (!reply && !acquireResult) goto finish;
                    break;

                case BR_DEAD_REPLY:
//                    err = DEAD_OBJECT;
                    goto finish;

                case BR_FAILED_REPLY:
//                    err = FAILED_TRANSACTION;
                    goto finish;

                case BR_ACQUIRE_RESULT: {
//                    ALOG_ASSERT(acquireResult != NULL, "Unexpected brACQUIRE_RESULT");
                    const int32_t result = mIn.readInt32();
                    if (!acquireResult) continue;
//                    *acquireResult = result ? NO_ERROR : INVALID_OPERATION;
                }
                    goto finish;

                case BR_REPLY: {
                    binder_transaction_data tr;
                    err = mIn.read(&tr, sizeof(tr));
//                    ALOG_ASSERT(err == NO_ERROR, "Not enough command data for brREPLY");
                    if (err != NO_ERROR) goto finish;

                    if (reply) {
                        if ((tr.flags & TF_STATUS_CODE) == 0) {
//                            reply->ipcSetDataReference(
//                                    reinterpret_cast<const uint8_t *>(tr.data.ptr.buffer),
//                                    tr.data_size,
//                                    reinterpret_cast<const binder_size_t *>(tr.data.ptr.offsets),
//                                    tr.offsets_size / sizeof(binder_size_t),
//                                    freeBuffer, this);
                        } else {
                            err = *reinterpret_cast<const status_t *>(tr.data.ptr.buffer);
//                            freeBuffer(NULL,
//                                       reinterpret_cast<const uint8_t *>(tr.data.ptr.buffer),
//                                       tr.data_size,
//                                       reinterpret_cast<const binder_size_t *>(tr.data.ptr.offsets),
//                                       tr.offsets_size / sizeof(binder_size_t), this);
                        }
                    } else {
//                        freeBuffer(NULL,
//                                   reinterpret_cast<const uint8_t *>(tr.data.ptr.buffer),
//                                   tr.data_size,
//                                   reinterpret_cast<const binder_size_t *>(tr.data.ptr.offsets),
//                                   tr.offsets_size / sizeof(binder_size_t), this);
                        continue;
                    }
                }
                    goto finish;

                default:
                    err = executeCommand(cmd);
                    if (err != NO_ERROR) goto finish;
                    break;
            }
        }

        finish:
        if (err != NO_ERROR) {
            if (acquireResult) *acquireResult = err;
            if (reply) reply->setError(err);
            mLastError = err;
        }

        return err;
    }

    BBinder *the_context_object;

    void setTheContextObject(BBinder *obj) {
        the_context_object = obj;
    }

    status_t IPCThreadState::executeCommand(int32_t cmd) {
        BBinder *obj;
//        RefBase::weakref_type* refs;
        status_t result = NO_ERROR;

        switch ((uint32_t) cmd) {
            case BR_ERROR:
                result = mIn.readInt32();
                break;

            case BR_OK:
                break;

            case BR_ACQUIRE:
//                refs = (RefBase::weakref_type*)mIn.readPointer();
//                obj = (BBinder*)mIn.readPointer();
//                ALOG_ASSERT(refs->refBase() == obj,
//                            "BR_ACQUIRE: object %p does not match cookie %p (expected %p)",
//                            refs, obj, refs->refBase());
//                obj->incStrong(mProcess.get());
//                IF_LOG_REMOTEREFS() {
//            LOG_REMOTEREFS("BR_ACQUIRE from driver on %p", obj);
//            obj->printRefs();
//              }
                mOut.writeInt32(BC_ACQUIRE_DONE);
//                mOut.writePointer((uintptr_t) refs);
//                mOut.writePointer((uintptr_t) obj);
                break;

            case BR_RELEASE:
//                refs = (RefBase::weakref_type *) mIn.readPointer();
//                obj = (BBinder *) mIn.readPointer();
//                ALOG_ASSERT(refs->refBase() == obj,
//                            "BR_RELEASE: object %p does not match cookie %p (expected %p)",
//                            refs, obj, refs->refBase());
//                IF_LOG_REMOTEREFS()
//                {
//                    LOG_REMOTEREFS("BR_RELEASE from driver on %p", obj);
//                    obj->printRefs();
//                }
//                mPendingStrongDerefs.push(obj);
                break;

            case BR_INCREFS:
//                refs = (RefBase::weakref_type *) mIn.readPointer();
//                obj = (BBinder *) mIn.readPointer();
//                refs->incWeak(mProcess.get());
//                mOut.writeInt32(BC_INCREFS_DONE);
//                mOut.writePointer((uintptr_t) refs);
//                mOut.writePointer((uintptr_t) obj);
                break;

            case BR_DECREFS:
//                refs = (RefBase::weakref_type *) mIn.readPointer();
//                obj = (BBinder *) mIn.readPointer();
                // NOTE: This assertion is not valid, because the object may no
                // longer exist (thus the (BBinder*)cast above resulting in a different
                // memory address).
                //ALOG_ASSERT(refs->refBase() == obj,
                //           "BR_DECREFS: object %p does not match cookie %p (expected %p)",
                //           refs, obj, refs->refBase());
//                mPendingWeakDerefs.push(refs);
                break;

            case BR_ATTEMPT_ACQUIRE:
//                refs = (RefBase::weakref_type *) mIn.readPointer();
//                obj = (BBinder *) mIn.readPointer();

//                {
//                    const bool success = refs->attemptIncStrong(mProcess.get());
//                    ALOG_ASSERT(success && refs->refBase() == obj,
//                                "BR_ATTEMPT_ACQUIRE: object %p does not match cookie %p (expected %p)",
//                                refs, obj, refs->refBase());

//                    mOut.writeInt32(BC_ACQUIRE_RESULT);
//                    mOut.writeInt32((int32_t) success);
//                }
                break;

            case BR_TRANSACTION: {
                binder_transaction_data tr;
                result = mIn.read(&tr, sizeof(tr));
//                ALOG_ASSERT(result == NO_ERROR,
//                            "Not enough command data for brTRANSACTION");
                if (result != NO_ERROR) break;

                Parcel buffer;
//                buffer.ipcSetDataReference(
//                        reinterpret_cast<const uint8_t *>(tr.data.ptr.buffer),
//                        tr.data_size,
//                        reinterpret_cast<const binder_size_t *>(tr.data.ptr.offsets),
//                        tr.offsets_size / sizeof(binder_size_t), freeBuffer, this);

                const pid_t origPid = mCallingPid;
                const uid_t origUid = mCallingUid;
                const int32_t origStrictModePolicy = mStrictModePolicy;
                const int32_t origTransactionBinderFlags = mLastTransactionBinderFlags;

                mCallingPid = tr.sender_pid;
                mCallingUid = tr.sender_euid;
                mLastTransactionBinderFlags = tr.flags;

                //ALOGI(">>>> TRANSACT from pid %d uid %d\n", mCallingPid, mCallingUid);

                Parcel reply;
                status_t error;
//                IF_LOG_TRANSACTIONS()
//                {
//                    TextOutput::Bundle _b(alog);
//                    alog << "BR_TRANSACTION thr " << (void *) pthread_self()
//                         << " / obj " << tr.target.ptr << " / code "
//                         << TypeCode(tr.code) << ": " << indent << buffer
//                         << dedent << endl
//                         << "Data addr = "
//                         << reinterpret_cast<const uint8_t *>(tr.data.ptr.buffer)
//                         << ", offsets addr="
//                         << reinterpret_cast<const size_t *>(tr.data.ptr.offsets) << endl;
//                }
                if (tr.target.ptr) {
                    // We only have a weak reference on the target object, so we must first try to
                    // safely acquire a strong reference before doing anything else with it.
//                    if (reinterpret_cast<RefBase::weakref_type *>(tr.target.ptr)->attemptIncStrong(this)) {
//                        error = reinterpret_cast<BBinder *>(tr.cookie)->transact(tr.code, buffer,
//                                                                                 &reply, tr.flags);
//                        reinterpret_cast<BBinder *>(tr.cookie)->decStrong(this);
//                    } else {
//                        error = UNKNOWN_TRANSACTION;
//                    }

                } else {
                    error = the_context_object->transact(tr.code, buffer, &reply, tr.flags);
                }

                //ALOGI("<<<< TRANSACT from pid %d restore pid %d uid %d\n",
                //     mCallingPid, origPid, origUid);

                if ((tr.flags & TF_ONE_WAY) == 0) {
//                    LOG_ONEWAY("Sending reply to %d!", mCallingPid);
                    if (error < NO_ERROR) reply.setError(error);
                    sendReply(reply, 0);
                } else {
//                    LOG_ONEWAY("NOT sending reply to %d!", mCallingPid);
                }

                mCallingPid = origPid;
                mCallingUid = origUid;
                mStrictModePolicy = origStrictModePolicy;
                mLastTransactionBinderFlags = origTransactionBinderFlags;

//                IF_LOG_TRANSACTIONS()
//                {
//                    TextOutput::Bundle _b(alog);
//                    alog << "BC_REPLY thr " << (void *) pthread_self() << " / obj "
//                         << tr.target.ptr << ": " << indent << reply << dedent << endl;
//                }

            }
                break;

            case BR_DEAD_BINDER: {
//                BpBinder *proxy = (BpBinder *) mIn.readPointer();
//                proxy->sendObituary();
//                mOut.writeInt32(BC_DEAD_BINDER_DONE);
//                mOut.writePointer((uintptr_t) proxy);
            }
                break;

            case BR_CLEAR_DEATH_NOTIFICATION_DONE: {
//                BpBinder *proxy = (BpBinder *) mIn.readPointer();
//                proxy->getWeakRefs()->decWeak(proxy);
            }
                break;

            case BR_FINISHED:
//                result = TIMED_OUT;
                break;

            case BR_NOOP:
                break;

            case BR_SPAWN_LOOPER:
                //创建binder普通线程
                mProcess->spawnPooledThread(false);
                break;

            default:
//                ALOGE("*** BAD COMMAND %d received from Binder driver\n", cmd);
//                result = UNKNOWN_ERROR;
                break;
        }

        if (result != NO_ERROR) {
            mLastError = result;
        }

        return result;
    }

    status_t IPCThreadState::sendReply(const Parcel &reply, uint32_t flags) {
        return 0;
    }

    void IPCThreadState::freeBuffer(Parcel *parcel, const uint8_t *data, size_t dataSize,
                                    const binder_size_t *objects, size_t objectsSize,
                                    void *cookie) {

    }

    status_t IPCThreadState::getAndExecuteCommand() {
        return 0;
    }


}
