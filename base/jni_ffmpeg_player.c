// #include <jni.h>

#include "player.h"
#include "logger.h"
#include <jni.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"

// for native window JNI
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include "videoDecode.h"

#define TAG "ffmpeg_jni"
#define DEBUG true

// com.xxx.ffmpeglibrary.FFmpegBridge
#define JNI_CLASS_FFMPEG_BRIDGE     "com/xxx/ffmpeglibrary/FFmpegBridge"

void base_hello(JNIEnv * env,jobject obj);

jstring get_str(JNIEnv* env, jobject thiz);

jint add_int(JNIEnv* env, jobject jobj, jint num1, jint num2);

jstring get_urlprotocolinfo(JNIEnv* env, jobject thiz);
jstring get_avformatinfo(JNIEnv* env, jobject thiz);
jstring get_avcodecinfo(JNIEnv* env, jobject thiz);
jstring get_avfilterinfo(JNIEnv* env, jobject thiz);
jstring get_configurationinfo(JNIEnv* env, jobject thiz);
jint decode(JNIEnv* env, jobject thiz, jstring input_jstr, jstring output_jstr);
jint stream(JNIEnv* env, jobject thiz, jstring input_jstr, jstring output_jstr);
jint SetSurface(JNIEnv* env, jobject thiz, jobject jsurface);
jint pausePlayer(JNIEnv* env, jobject thiz);
jint resumePlayer(JNIEnv* env, jobject thiz);
jint stopPlayer(JNIEnv* env, jobject thiz);

static JNINativeMethod gMethods[] =
{
    {"_baseHello",             "()V",                          (void *)base_hello },
    {"getStr",                 "()Ljava/lang/String;",         (void*)get_str },
    {"addInt",                 "(II)I",                        (void*)add_int },
    {"urlprotocolinfo",        "()Ljava/lang/String;",         (void *)get_urlprotocolinfo },
    {"avformatinfo",           "()Ljava/lang/String;",         (void*)get_avformatinfo },
    {"avcodecinfo",            "()Ljava/lang/String;",         (void*)get_avcodecinfo },
    {"avfilterinfo",           "()Ljava/lang/String;",         (void*)get_avfilterinfo },
    {"configurationinfo",      "()Ljava/lang/String;",         (void*)get_configurationinfo },
    {"decode",                 "(Ljava/lang/String;Ljava/lang/String;)I", (void*)decode},
	{"stream",                 "(Ljava/lang/String;Ljava/lang/String;)I", (void*)stream},
	{"nativeSetSurface",       "(Landroid/view/Surface;)I", (void*)SetSurface},
	{"nativePausePlayer",      "()I", (void*)pausePlayer},
	{"nativeResumePlayer",     "()I", (void*)resumePlayer},
	{"nativeStopPlayer",       "()I", (void*)stopPlayer}
};

static int registerFFplayer(JNIEnv *env, jclass cls){
    LogI(TAG, DEBUG, "registerNatives");
    //调用RegisterNatives来注册本地方法，完成映射
    if((*env)->RegisterNatives(env, cls, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < 0){
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
    LogE(TAG, DEBUG, "JNI_OnLoad");
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
    LogI(TAG,DEBUG,"base_hello!!!");
    return;
}

jstring get_str(JNIEnv* env, jobject thiz)
{
    return (*env)->NewStringUTF(env, "get_str");
}
jint add_int(JNIEnv* env, jobject jobj, jint num1, jint num2)
{
    return num1 + num2;
}
jstring get_urlprotocolinfo(JNIEnv* env, jobject thiz)
{
    return (*env)->NewStringUTF(env, "get_urlprotocolinfo");
}
jstring get_avformatinfo(JNIEnv* env, jobject thiz)
{
    return (*env)->NewStringUTF(env, "get_avformatinfo");
}
jstring get_avcodecinfo(JNIEnv* env, jobject thiz)
{
    return (*env)->NewStringUTF(env, "get_avcodecinfo");
}
jstring get_avfilterinfo(JNIEnv* env, jobject thiz)
{
    return (*env)->NewStringUTF(env, "get_avfilterinfo");
}
jstring get_configurationinfo(JNIEnv* env, jobject thiz)
{
    return (*env)->NewStringUTF(env, "get_configurationinfo");
}
jint decode(JNIEnv* env, jobject thiz, jstring input_jstr, jstring output_jstr)
{
    return 0;
}
jint stream(JNIEnv* env, jobject thiz, jstring input_jstr, jstring output_jstr)
{
	return 0;
}

static ANativeWindow* mANativeWindow;
static ANativeWindow_Buffer mBuffer;

jint SetSurface(JNIEnv* env, jobject thiz, jobject surface)
{
    LogE(TAG,DEBUG, "SetSurface.\n");

    if (NULL == surface) {
        LogE(TAG,DEBUG, "surface is null, destroy?");
        mANativeWindow = NULL;
        return 0;
    }
    mANativeWindow = ANativeWindow_fromSurface(env,surface);
    if(mANativeWindow == NULL){
        LogE(TAG,DEBUG, "mANativeWindow is null");
    }
    pthread_t thread;
    LogE(TAG,DEBUG, "pthread_create open_media");
    pthread_create(&thread, NULL, open_media, NULL);
    return 0;
}


int32_t setBuffersGeometry(int32_t width, int32_t height)
{
    int32_t format = WINDOW_FORMAT_RGB_565;

    if (NULL == mANativeWindow) {
        LogE(TAG,DEBUG, "mANativeWindow is NULL.");
        return -1;
    }

    return ANativeWindow_setBuffersGeometry(mANativeWindow, width, height,
            format);
}

void renderSurface(uint8_t *pixel)
{
    LogI(TAG,DEBUG, "renderSurface ");
    ANativeWindow_acquire(mANativeWindow);
    LogI(TAG,DEBUG, "renderSurface ANativeWindow_acquire");
    if(0 != ANativeWindow_lock(mANativeWindow, &mBuffer, NULL))
    {
        LogE(TAG,DEBUG, "ANativeWindow_lock error");
    }
    LogI(TAG,DEBUG, "mBuffer width is %d, height is %d, stride is %d", mBuffer.width, mBuffer.height, mBuffer.stride);
    if(mBuffer.width >= mBuffer.stride)
    {
        memcpy(mBuffer.bits, pixel, mBuffer.width * mBuffer.height * 2);
    }
    else
    {
        int i;
        for(i = 0; i < mBuffer.height; i++)
        {
            // LogI(TAG,DEBUG, "renderSurface i:%d",i);
            memcpy((void*)((int)mBuffer.bits + mBuffer.stride * i * 2),
                    (void*)((int)pixel + mBuffer.width * i * 2),
                    mBuffer.width * 2);
        }
    }
    LogI(TAG,DEBUG, "renderSurface ANativeWindow_unlockAndPost");
    if(0 != ANativeWindow_unlockAndPost(mANativeWindow))
    {
        LogE(TAG,DEBUG, "ANativeWindow_unlockAndPost error");
    }
    LogI(TAG,DEBUG, "renderSurface ANativeWindow_release");
    ANativeWindow_release(mANativeWindow);
}

jint pausePlayer(JNIEnv* env, jobject thiz)
{
    LogE(TAG,DEBUG, "pausePlayer.\n");
    return 0;
}

jint resumePlayer(JNIEnv* env, jobject thiz)
{
    LogE(TAG,DEBUG, "resumePlayer.\n");
    return 0;
}

jint stopPlayer(JNIEnv* env, jobject thiz)
{
    LogE(TAG,DEBUG, "stopPlayer.\n");
    return 0;
}
