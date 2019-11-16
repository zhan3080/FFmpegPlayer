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

    public void sayHello() {
        Log.i(TAG, "sayHello");
        _baseHello();
        Log.i(TAG, "sayHello libPath:" + System.getProperty(libName));
    }
}
