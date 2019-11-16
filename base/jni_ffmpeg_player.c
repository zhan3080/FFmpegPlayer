// #include <jni.h>

#include "logger.h"
#include <jni.h>
#define TAG "ffmpeg_jni"
#define DEBUG true

// com.xxx.ffmpeglibrary.FFmpegBridge
#define JNI_CLASS_FFMPEG_BRIDGE     "com/xxx/ffmpeglibrary/FFmpegBridge"


 /* Declare function map to java native method 声明映射过来的本地函数 */
 void base_hello(JNIEnv * env,jobject obj);

/* rule of JNINativeMethod 
  1、java类方法名
  2、signature 方法签名，表示一个java跟native方法参数的映射关系
  3、本地方法名
*/
static JNINativeMethod g_methods[] =
{
    {"_baseHello",             "(V)V",       (void *)base_hello}
    // {"_setSurface",         "(JLandroid/view/Surface;)V",   (void *)set_surface},
    // {"_setFrameData",      "(Ljava/lang/String;I[BI)V",     (void*)set_frame_data},
    // {"_prepare",            "(J)V",                         (void *)prepare},
    // {"_start",              "(J)V",                         (void *)start},
    // {"_stop",               "(J)V",                         (void *)stop},
    // {"_getVideoWidth",      "(J)I",                         (void *)get_video_width},
    // {"_getVideoHeight",     "(J)I",                         (void *)get_video_height},
    // {"_getFps",             "(J)[J",                        (void *)get_fps},
    // {"_getDelay",           "(JI)I",                        (void *)get_delay},
    // {"_setVolume",          "(JI)I",                        (void *)set_volume},
    // {"_pause",              "(J)I",                         (void *)pause}

};

static int registerFFplayer(JNIEnv *env, jclass cls){
    LogI(TAG, DEBUG, "registerNatives");
    //调用RegisterNatives来注册本地方法，完成映射
    if((*env)->RegisterNatives(env, cls, g_methods, sizeof(g_methods) / sizeof(g_methods[0])) < 0){
        LogE(TAG, DEBUG, "RegisterNatives failed");
        return 1;
    }
    return 0;
}

/* This function will be exec when so been loading. 
   这个方法在so加载的时候执行
*/
jint JNI_OnLoad(JavaVM* vm, void *reserved)
{
    JNIEnv* env = NULL;
    if((*vm)->GetEnv(vm,(void **)&env,JNI_VERSION_1_4) != JNI_OK){
        LogE(TAG, DEBUG, "JNI_OnLoad GetEnv error");
        return -1;
    }

    jclass cls = (*env)->FindClass(env,JNI_CLASS_FFMPEG_BRIDGE);//通过类路径字符串找到对应类
    registerFFplayer(env,cls);
    LogE(TAG, DEBUG, "JNI_OnLoad success");
    return JNI_VERSION_1_4;
}

void JNI_OnUnload(JavaVM* vm, void *reserved)
{
    LogI(TAG, DEBUG, "JNI_OnUnload");
    return;
}

void base_hello(JNIEnv * env,jobject obj)
{
    LogI(TAG,DEBUG,"base_hello");
    return;
}




