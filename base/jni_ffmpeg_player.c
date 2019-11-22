// #include <jni.h>

#include "logger.h"
#include <jni.h>
#define TAG "ffmpeg_jni"
#define DEBUG true

// com.xxx.ffmpeglibrary.FFmpegBridge
#define JNI_CLASS_FFMPEG_BRIDGE     "com/xxx/ffmpeglibrary/FFmpegBridge"


/* Declare function map to java native method 声明映射过来的本地函数 */
void base_hello(JNIEnv * env,jobject obj);

jstring get_str(JNIEnv* env, jobject thiz);

jint add_int(JNIEnv* env, jobject jobj, jint num1, jint num2);

/* rule of JNINativeMethod 
*  方法对应表
*  1、java类方法名
*  2、signature 方法签名，表示一个java跟native方法参数的映射关系
*  3、本地方法名
*/


 
static JNINativeMethod gMethods[] =
{
    {"_baseHello",             "()V",                          (void *)base_hello },
    {"getStr",                 "()Ljava/lang/String;",         (void*)get_str },
    {"addInt",                 "(II)I",                        (void*)add_int }

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
    return (*env)->NewStringUTF(env, "I am chenyu, 动态注册JNI");
}
 
jint add_int(JNIEnv* env, jobject jobj, jint num1, jint num2){
    return num1 + num2;
}




