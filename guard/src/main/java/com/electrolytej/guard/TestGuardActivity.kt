package com.electrolytej.guard

import android.app.Activity
import android.content.Context
import android.content.pm.IPackageManager
import android.content.pm.PackageManager
import android.os.Bundle
import android.os.ServiceManager
import android.provider.Settings
import android.text.TextUtils
import android.util.Log
import android.view.IWindowManager
import android.widget.TextView
import androidx.annotation.Keep
import com.electrolytej.common.ToolBox.sha1ToHexString
import com.electrolytej.guard.security.SecurityChecker
import java.io.File
import java.lang.reflect.Proxy

class TestGuardActivity : Activity() {

    companion object {

        init {
            System.loadLibrary("guard")
        }

        const val TAG = "cjf_defense"

    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_testguard)


        findViewById<TextView>(R.id.bt_hook_frida).setOnClickListener {
            (it as TextView).text = stringFromJNI()
            testEvent(this)
        }
        findViewById<TextView>(R.id.bt_hook_frida2).setOnClickListener {

            (it as TextView).text = verify(this).toString()
        }
        var info = packageManager.getPackageInfo(packageName, PackageManager.GET_SIGNATURES)
        var bs = info.signatures[0].toByteArray()
        var s =
            "java 第一种获取签名的方法：firstInstallTime:${info.firstInstallTime} lastUpdateTime:${info.lastUpdateTime} ${
                sha1ToHexString(bs)
            }"
        Log.i(TAG, s)

        val package_b = ServiceManager.getService("package")
        val window_b = ServiceManager.getService("window")
        info = IPackageManager.Stub.asInterface(package_b)
            .getPackageInfo(packageName, PackageManager.GET_SIGNATURES, 0)
        bs = info.signatures[0].toByteArray()
        s =
            "java 第二种获取签名的方法：firstInstallTime:${info.firstInstallTime} lastUpdateTime:${info.lastUpdateTime} ${
                sha1ToHexString(bs)
            }"
        Log.i(TAG, s)

        Log.i(
            TAG,
            "IPackageManager isProxyClass:${Proxy.isProxyClass(IPackageManager.Stub.asInterface(package_b)::class.java)} " +
                    "IBinder isProxyClass:${Proxy.isProxyClass(package_b.javaClass)} " +
                    "IWindowManager isProxyClass：${
                        Proxy.isProxyClass(
                            IWindowManager.Stub.asInterface(
                                window_b
                            ).javaClass
                        )
                    }"
        )
//        printAllCalsses(packageName)
//        isMainProcess(this)
        Log.i(TAG, " prop:${getSysProp("ro.product.cpu.abi")}")
//        Timer(TAG).schedule(object : TimerTask() {
//            override fun run() {
//
//            }
//        }, 20_000, 24*3600*1000)
        Log.i(TAG, " verify:${verify(this)}")
        val substring = packageCodePath.substring(0, packageCodePath.lastIndexOf(47.toChar()))
        var soLib = "${applicationInfo.nativeLibraryDir}/libdynamic_so.so"
//        Log.i(TAG, "p $substring $packageCodePath $soLib ")
        Log.i(TAG, "soLib： $soLib ")
        Log.i(TAG, " dynamicLoader:${dynamicLoader(soLib)}")
        main(this)

        val string1 = Settings.Secure.getString(contentResolver, Settings.Secure.ANDROID_ID)
        val string = Settings.System.getString(contentResolver, Settings.System.ANDROID_ID)
        Log.d("cjf", "android_id $string $string1")
        val securityChecker = SecurityChecker(this)
        Log.d("cjf_defense","verify apk:"+securityChecker.verifyApk(File(packageCodePath)))
        testServiceManager()
    }

    private fun testServiceManager() {
//        com.jamesfchen.binder.ServiceManager.addService("cjf_binder", MyBinder())
    }

    fun getSysProp(str: String): String {
        return if (TextUtils.isEmpty(str)) {
            ""
        } else try {
            val cls = Class.forName("android.os.SystemProperties")
            cls.getDeclaredMethod("get", String::class.java).invoke(cls, str) as String
        } catch (th: Throwable) {
            ""
        }
    }

    fun stringFromJava() = "string  from java"

    @Keep
    external fun stringFromJNI(): String

    @Keep
    external fun main(ctx: Context): Boolean

    @Keep
    external fun verify(ctx: Context): Boolean

    @Keep
    external fun dynamicLoader(path: String): Boolean
    @Keep
    external fun testEvent(ctx: Context): Boolean

}
