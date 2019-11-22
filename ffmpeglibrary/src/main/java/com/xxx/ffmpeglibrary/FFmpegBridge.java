package com.xxx.ffmpeglibrary;

import android.util.Log;

public class FFmpegBridge {
    private static final String TAG = "FFmpegBridge";
    //jni库名称
    public static String libName = "ffplayer";

    static {
        System.loadLibrary(libName);
    }

    public native void _baseHello();

    public native String getStr();

    public native int addInt(int a, int b);

    public void sayHello() {
        Log.i(TAG, "sayHello");
        _baseHello();
        Log.i(TAG, "sayHello getStr:" + getStr());
        Log.i(TAG, "sayHello addInt:" + addInt(1, 3));
    }
}
