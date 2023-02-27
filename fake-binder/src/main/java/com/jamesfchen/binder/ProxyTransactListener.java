package com.jamesfchen.binder;

import android.os.IBinder;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

/**
 * Copyright ® $ 2021
 * All right reserved.
 *
 * @author jamesfchen
 * @email hawksjamesf@gmail.com
 * @since 8月/04/2021  周三
 */
public interface ProxyTransactListener {
    /**
     * Called before onTransact.
     *
     * @return an object that will be passed back to #onTransactEnded (or null).
     * @hide
     */
    @Nullable
    default Object onTransactStarted(@NonNull IBinder binder, int transactionCode, int flags) {
        return onTransactStarted(binder, transactionCode);
    }

    /**
     * Called before onTransact.
     *
     * @return an object that will be passed back to #onTransactEnded (or null).
     */
    @Nullable
    Object onTransactStarted(@NonNull IBinder binder, int transactionCode);

    /**
     * Called after onTranact (even when an exception is thrown).
     *
     * @param session The object return by #onTransactStarted.
     */
    void onTransactEnded(@Nullable Object session);
}