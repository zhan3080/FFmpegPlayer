package com.xxx.ffmpeglibrary;

import android.util.Log;
import android.view.Surface;

public class FFmpegBridge {
    private static final String TAG = "FFmpegBridge";
    //jni库名称
    public static String libName = "ffplayer";
    public static String libavcodec = "avcodec-58";
    public static String libavdevice = "avdevice-58";
    public static String libavfilter = "avfilter-7";
    public static String libavformat = "avformat-58";
    public static String libavutil = "avutil-56";
    public static String libpostproc = "postproc-55";
    public static String libswresample = "swresample-3";
    public static String libswscale = "swscale-5";

    static {
//        System.loadLibrary(libavcodec);
//        System.loadLibrary(libavdevice);
//        System.loadLibrary(libavfilter);
//        System.loadLibrary(libavformat);
//        System.loadLibrary(libavutil);
//        System.loadLibrary(libpostproc);
//        System.loadLibrary(libswresample);
//        System.loadLibrary(libswscale);
        System.loadLibrary(libName);
    }

    public native void _baseHello();

    public native String getStr();

    public native int addInt(int a, int b);

    public native String urlprotocolinfo();
    public native String avformatinfo();
    public native String avcodecinfo();
    public native String avfilterinfo();
    public native String configurationinfo();
    public native int decode(String inputUrl, String outputUrl);
    public native int stream(String inputurl, String outputurl);

    public native int nativeSetSurface(Surface view);

    public native int nativePausePlayer();

    public native int nativeResumePlayer();

    public native int nativeStopPlayer();

    public void sayHello() {
        Log.i(TAG, "sayHello!!");
        _baseHello();
        Log.i(TAG, "sayHello getStr:" + getStr());
        Log.i(TAG, "sayHello addInt:" + addInt(1, 3));
    }

    public String getUrlprotocolinfo() {
        return urlprotocolinfo();
    }

    public String getAvformatinfo() {
        return avformatinfo();
    }

    public String getAvcodecinfo() {
        return avcodecinfo();
    }

    public String getAvfilterinfo() {
        return avfilterinfo();
    }

    public String getConfigurationinfo() {
        return configurationinfo();
    }

    public  int setSurface(Surface view){
        return nativeSetSurface(view);
    }

    public  int pausePlayer(){
        return nativePausePlayer();
    }

    public  int resumePlayer(){
        return nativeResumePlayer();
    }

    public  int stopPlayer(){
        return nativeStopPlayer();
    }
}
