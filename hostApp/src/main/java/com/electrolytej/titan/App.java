package com.electrolytej.titan;

import android.app.Application;
import android.content.Context;
import android.util.Log;

import com.s.a.H;

public class App extends Application {
    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        Log.e("cjf_attack", "App attachBaseContext:" + base.getClass().getCanonicalName());
//        System.loadLibrary("gadget");
//        VirtualAppInitiator.attachApplicationContext(this);
        String soLib = getFilesDir().getParent() + "/lib/libbiz.so";
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.e("cjf_attack", "App onCreate：" + getBaseContext().getClass().getCanonicalName() + " " + getApplicationContext().getClass().getCanonicalName());
    }
}
