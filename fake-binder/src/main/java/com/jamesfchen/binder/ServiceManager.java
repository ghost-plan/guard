package com.jamesfchen.binder;

import android.os.IBinder;
import android.os.RemoteException;

import com.facebook.jni.HybridData;
import com.facebook.soloader.nativeloader.NativeLoader;
import com.facebook.soloader.nativeloader.SystemDelegate;

/**
 * Copyright ® $ 2021
 * All right reserved.
 *
 * @author jamesfchen
 * @email hawksjamesf@gmail.com
 * @since 8月/04/2021  周三
 */
public class ServiceManager {
    private static final int ADD_SERVICE_TRANSACTION = 1;
    private static final int GET_SERVICE_TRANSACTION = 2;
    private static BinderProxy mRemote;

    static {
        if (!NativeLoader.isInitialized()) {
            NativeLoader.init(new SystemDelegate());
        }
        NativeLoader.loadLibrary("fakebinder");
    }

    HybridData hybridData;

    public ServiceManager() {
        hybridData = initHybrid();
    }

    public static ServiceManager getInstance() {
        return Holder.sInstance;
    }

    private static native HybridData initHybrid();

    private native BinderProxy getBinderProxy();

    public void addService(String name, IBinder service) throws RemoteException {
        if (mRemote == null) {
            mRemote = getBinderProxy();
        }
        Parcel data = Parcel.obtain();
        Parcel reply = Parcel.obtain();
//        data.writeInterfaceToken(IServiceManager.descriptor);
        data.writeString(name);
        data.writeStrongBinder(service);
        mRemote.transact(ADD_SERVICE_TRANSACTION, data, reply, 0);
        reply.recycle();
        data.recycle();
    }

    static class Holder {
        public static ServiceManager sInstance = new ServiceManager();

    }
}
