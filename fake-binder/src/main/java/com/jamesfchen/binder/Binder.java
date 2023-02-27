package com.jamesfchen.binder;

import android.os.IBinder;
import android.os.IInterface;
import android.os.Parcel;
import android.os.RemoteException;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.FileDescriptor;
import java.util.NoSuchElementException;

/**
 * Copyright ® $ 2021
 * All right reserved.
 *
 * @author jamesfchen
 * @email hawksjamesf@gmail.com
 * @since 8月/03/2021  周二
 */
public class Binder implements IBinder {

    /**
     * Get the canonical name of the interface supported by this binder.
     */
    @Nullable
    @Override
    public String getInterfaceDescriptor() throws RemoteException {
        return null;
    }

    /**
     * Check to see if the object still exists.
     *
     * @return Returns false if the
     * hosting process is gone, otherwise the result (always by default
     * true) returned by the pingBinder() implementation on the other
     * side.
     */
    @Override
    public boolean pingBinder() {
        return false;
    }

    /**
     * Check to see if the process that the binder is in is still alive.
     *
     * @return false if the process is not alive.  Note that if it returns
     * true, the process may have died while the call is returning.
     */
    @Override
    public boolean isBinderAlive() {
        return false;
    }

    /**
     * Attempt to retrieve a local implementation of an interface
     * for this Binder object.  If null is returned, you will need
     * to instantiate a proxy class to marshall calls through
     * the transact() method.
     *
     * @param descriptor
     */
    @Nullable
    @Override
    public IInterface queryLocalInterface(@NonNull String descriptor) {
        return null;
    }

    /**
     * Print the object's state into the given stream.
     *
     * @param fd   The raw file descriptor that the dump is being sent to.
     * @param args additional arguments to the dump request.
     */
    @Override
    public void dump(@NonNull FileDescriptor fd, @Nullable String[] args) throws RemoteException {

    }

    /**
     * Like {@link #dump(FileDescriptor, String[])} but always executes
     * asynchronously.  If the object is local, a new thread is created
     * to perform the dump.
     *
     * @param fd   The raw file descriptor that the dump is being sent to.
     * @param args additional arguments to the dump request.
     */
    @Override
    public void dumpAsync(@NonNull FileDescriptor fd, @Nullable String[] args) throws RemoteException {

    }

    @Override
    public boolean transact(int code, Parcel data, Parcel reply, int flags) {
        return false;
    }

    /**
     * Register the recipient for a notification if this binder
     * goes away.  If this binder object unexpectedly goes away
     * (typically because its hosting process has been killed),
     * then the given {@link DeathRecipient}'s
     * {@link DeathRecipient#binderDied DeathRecipient.binderDied()} method
     * will be called.
     *
     * <p>You will only receive death notifications for remote binders,
     * as local binders by definition can't die without you dying as well.
     *
     * @param recipient
     * @param flags
     * @throws RemoteException if the target IBinder's
     *                         process has already died.
     * @see #unlinkToDeath
     */
    @Override
    public void linkToDeath(@NonNull DeathRecipient recipient, int flags) throws RemoteException {

    }

    /**
     * Remove a previously registered death notification.
     * The recipient will no longer be called if this object
     * dies.
     *
     * @param recipient
     * @param flags
     * @return {@code true} if the <var>recipient</var> is successfully
     * unlinked, assuring you that its
     * {@link DeathRecipient#binderDied DeathRecipient.binderDied()} method
     * will not be called;  {@code false} if the target IBinder has already
     * died, meaning the method has been (or soon will be) called.
     * @throws NoSuchElementException if the given
     *                                <var>recipient</var> has not been registered with the IBinder, and
     *                                the IBinder is still alive.  Note that if the <var>recipient</var>
     *                                was never registered, but the IBinder has already died, then this
     *                                exception will <em>not</em> be thrown, and you will receive a false
     *                                return value instead.
     */
    @Override
    public boolean unlinkToDeath(@NonNull DeathRecipient recipient, int flags) {
        return false;
    }
}
