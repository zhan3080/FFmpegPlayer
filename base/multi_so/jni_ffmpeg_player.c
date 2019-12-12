// #include <jni.h>

#include "logger.h"
#include <jni.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#define TAG "ffmpeg_jni"
#define DEBUG true

// com.xxx.ffmpeglibrary.FFmpegBridge
#define JNI_CLASS_FFMPEG_BRIDGE     "com/xxx/ffmpeglibrary/FFmpegBridge"


/* Declare function map to java native method 声明映射过来的本地函数 */
void base_hello(JNIEnv * env,jobject obj);

jstring get_str(JNIEnv* env, jobject thiz);

jint add_int(JNIEnv* env, jobject jobj, jint num1, jint num2);

jstring get_urlprotocolinfo(JNIEnv* env, jobject thiz);
jstring get_avformatinfo(JNIEnv* env, jobject thiz);
jstring get_avcodecinfo(JNIEnv* env, jobject thiz);
jstring get_avfilterinfo(JNIEnv* env, jobject thiz);
jstring get_configurationinfo(JNIEnv* env, jobject thiz);

/* rule of JNINativeMethod
*  方法对应表
*  1、java类方法名
*  2、signature 方法签名，表示一个java跟native方法参数的映射关系
*  3、本地方法名
*/



/*static JNINativeMethod gMethods[] =
{
    {"_baseHello",             "()V",                          (void *)base_hello },
    {"getStr",                 "()Ljava/lang/String;",         (void*)get_str },
    {"addInt",                 "(II)I",                        (void*)add_int }

};*/

static JNINativeMethod gMethods[] =
{
    {"_baseHello",             "()V",                          (void *)base_hello },
    {"getStr",                 "()Ljava/lang/String;",         (void*)get_str },
    {"addInt",                 "(II)I",                        (void*)add_int },
    {"urlprotocolinfo",        "()Ljava/lang/String;",         (void *)get_urlprotocolinfo },
    {"avformatinfo",           "()Ljava/lang/String;",         (void*)get_avformatinfo },
    {"avcodecinfo",            "()Ljava/lang/String;",         (void*)get_avcodecinfo },
    {"avfilterinfo",           "()Ljava/lang/String;",         (void*)get_avfilterinfo },
    {"configurationinfo",      "()Ljava/lang/String;",         (void*)get_configurationinfo }

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
    char info[10000] = { 0 };
    sprintf(info, "%s\n", avcodec_configuration());
    return (*env)->NewStringUTF(env, info);

    //return (*env)->NewStringUTF(env, "I am chenyu, 动态注册JNI");
}

jint add_int(JNIEnv* env, jobject jobj, jint num1, jint num2)
{
    return num1 + num2;
}

jstring get_urlprotocolinfo(JNIEnv* env, jobject thiz)
{
    //return (*env)->NewStringUTF(env, "I am chenyu, 动态注册JNI");
    char info[40000]={0};
	//av_register_all();

	struct URLProtocol *pup = NULL;
	//Input
	struct URLProtocol **p_temp = &pup;
	avio_enum_protocols((void **)p_temp, 0);
	while ((*p_temp) != NULL){
		sprintf(info, "%s[In ][%10s]\n", info, avio_enum_protocols((void **)p_temp, 0));
	}
	pup = NULL;
	//Output
	avio_enum_protocols((void **)p_temp, 1);
	while ((*p_temp) != NULL){
		sprintf(info, "%s[Out][%10s]\n", info, avio_enum_protocols((void **)p_temp, 1));
	}
	return (*env)->NewStringUTF(env, info);
}

jstring get_avformatinfo(JNIEnv* env, jobject thiz)
{
    char info[40000] = { 0 };

    //av_register_all();

    AVInputFormat *if_temp = av_iformat_next(NULL);
    AVOutputFormat *of_temp = av_oformat_next(NULL);
    //Input
    while(if_temp!=NULL){
    	sprintf(info, "%s[In ][%10s]\n", info, if_temp->name);
    	if_temp=if_temp->next;
    }
    //Output
    while (of_temp != NULL){
    	sprintf(info, "%s[Out][%10s]\n", info, of_temp->name);
    	of_temp = of_temp->next;
    }
    //LogE("%s", info);
    return (*env)->NewStringUTF(env, info);
}

jstring get_avcodecinfo(JNIEnv* env, jobject thiz)
{
    char info[40000] = { 0 };

    //av_register_all();

    AVCodec *c_temp = av_codec_next(NULL);

    while(c_temp!=NULL){
    	if (c_temp->decode!=NULL){
    		sprintf(info, "%s[Dec]", info);
    	}
    	else{
    		sprintf(info, "%s[Enc]", info);
    	}
    	switch (c_temp->type){
    	case AVMEDIA_TYPE_VIDEO:
    		sprintf(info, "%s[Video]", info);
    		break;
    	case AVMEDIA_TYPE_AUDIO:
    		sprintf(info, "%s[Audio]", info);
    		break;
    	default:
    		sprintf(info, "%s[Other]", info);
    		break;
    	}
    	sprintf(info, "%s[%10s]\n", info, c_temp->name);


    	c_temp=c_temp->next;
    }
    //LogE("%s", info);

    return (*env)->NewStringUTF(env, info);
}

jstring get_avfilterinfo(JNIEnv* env, jobject thiz)
{
    LogI(TAG,DEBUG,"get_avfilterinfo");
    char info[40000] = { 0 };
    LogI(TAG,DEBUG,"avfilter_register_all");
    //avfilter_register_all();
    AVFilter *f_temp = (AVFilter *)avfilter_next(NULL);
    int line = 0;
    while (f_temp != NULL && line < 10){
        LogI(TAG,DEBUG,"f_temp %s",info);
    	sprintf(info, "%s[%10s]\n", info, f_temp->name);
    	line++;
    }
    LogI(TAG,DEBUG,"%s", info);

    return (*env)->NewStringUTF(env, info);
}

jstring get_configurationinfo(JNIEnv* env, jobject thiz)
{
    char info[10000] = { 0 };
    //av_register_all();

    sprintf(info, "%s\n", avcodec_configuration());

    //LogE("%s", info);
    return (*env)->NewStringUTF(env, info);
}





