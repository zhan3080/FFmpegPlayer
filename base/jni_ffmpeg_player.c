// #include <jni.h>
#include "logger.h"
#include <jni.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"

// for native window JNI
#include <android/native_window_jni.h>
#include <android/native_window.h>

#include <pthread.h>

#define TAG "ffmpeg_jni"
#define DEBUG true

// com.xxx.ffmpeglibrary.FFmpegBridge
#define JNI_CLASS_FFMPEG_BRIDGE     "com/xxx/ffmpeglibrary/FFmpegBridge"

#define TEST_URL "/sdcard/test.mp4"

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
void* play_media(void *argv);

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
    LogE(TAG,DEBUG, "pthread_create play_media");
    pthread_create(&thread, NULL, play_media, NULL);
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

static int img_covert(AVPicture *dst, int dst_pix_fmt, const AVPicture *src, int src_pix_fmt, int src_width, int src_height)
{
    LogI(TAG, DEBUG, "open_media img_covert");
    int w = src_width;
    int h = src_height;
    struct SwsContext *pSwsCtx;
    pSwsCtx = sws_getContext(w, h, src_pix_fmt, w, h,dst_pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
    sws_scale(pSwsCtx, (const uint8_t* const *)src->data, src->linesize, 0, h, dst->data, dst->linesize);

    return 0;
}

AVCodecContext *acodec_ctx;
AVCodecContext *vcodec_ctx;
AVStream *vstream;
AVCodec *vcodec;

void* play_media(void *argv)
{
    LogI(TAG, DEBUG, "play_media begin");
    int i = 0;
    int err = 0;
    int framecnt = 0;

    AVFormatContext *fmt_ctx = NULL;
    AVDictionaryEntry *dict = NULL;
    AVCodecParameters *codecParameters = NULL;
    AVPacket *m_Packet;
    AVFrame *m_Frame;
    int video_stream_index = -1;
    pthread_t thread;

    avfilter_register_all();
    av_register_all();
    avformat_network_init();
    //1.创建封装格式上下文
    fmt_ctx = avformat_alloc_context();

    //2.打开输入文件，解封装
    err = avformat_open_input(&fmt_ctx, TEST_URL, NULL, NULL);
    if(err < 0)
    {
        LogE(TAG, DEBUG, "play_media avformat_open_input fail err:" + err);
        goto failure;
    }
    LogI(TAG, DEBUG, "play_media avformat_find_stream_info");
    //3.获取音视频流信息
    if((err == avformat_find_stream_info(fmt_ctx,NULL)) < 0)
    {
        LogE(TAG, DEBUG, "play_media avformat_find_stream_info fail err:" + err);
        goto failure;
    }

    //4.获取音视频流索引
    for(i = 0; i < fmt_ctx->nb_streams; i++)
    {
        LogE(TAG, DEBUG, "play_media codec_type %d",fmt_ctx->streams[i]->codec->codec_type);
        if(fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_stream_index = i;
        }
    }
    LogE(TAG, DEBUG, "play_media video_stream_index %d",video_stream_index);

    if(-1 != video_stream_index)
    {
        //5.获取解码器参数
        codecParameters = fmt_ctx->streams[video_stream_index]->codecpar;
        vcodec = avcodec_find_decoder(codecParameters->codec_id);
        //vstream = fmt_ctx->streams[video_stream_index];
        //vcodec_ctx = vstream->codec;
        if(NULL == vcodec)
        {
            LogE(TAG, DEBUG, "play_media avcodec_find_decoder fail");
            goto failure;
        }

        //7.创建解码器上下文
        vcodec_ctx = avcodec_alloc_context3(vcodec);
        if(NULL == vcodec_ctx)
        {
            LogE(TAG, DEBUG, "play_media avcodec_alloc_context3 fail");
            goto failure;
        }

        //todo
        if(avcodec_parameters_to_context(vcodec_ctx, codecParameters) != 0) 
        {
            LogE(TAG, DEBUG, "play_media avcodec_parameters_to_context fail");
            goto failure;
        }

        if(avcodec_open2(vcodec_ctx,vcodec, NULL) < 0)
        {
            LogE(TAG, DEBUG, "play_media avcodec_open2 fail");
            goto failure;
        }

        if((vcodec_ctx->width > 0) && (vcodec_ctx->height > 0))
        {
            setBuffersGeometry(vcodec_ctx->width, vcodec_ctx->height);
        }

        //9.创建存储编码数据和解码数据的结构体
        m_Packet = av_packet_alloc(); //创建 AVPacket 存放编码数据
        m_Frame = av_frame_alloc(); //创建 AVFrame 存放解码后的数据
        //10.解码循环
        while (av_read_frame(fmt_ctx, m_Packet) >= 0) 
        { //读取帧
            if (m_Packet->stream_index == video_stream_index) 
            {
                if (avcodec_send_packet(vcodec_ctx, m_Packet) != 0) 
                { //视频解码
                    LogE(TAG, DEBUG, "play_media avcodec_send_packet fail");
                    goto failure;
                }
                while (avcodec_receive_frame(vcodec_ctx, m_Frame) == 0) 
                {
                    //获取到 m_Frame 解码数据，在这里进行格式转换，然后进行渲染，下一节介绍 ANativeWindow 渲染过程
                    AVPicture pict;
                    uint8_t *dst_data[4];
                    int dst_linesize[4];

                    LogI(TAG, DEBUG, "play_media av_image_alloc");
                    av_image_alloc(pict.data, pict.linesize, vcodec_ctx->width, vcodec_ctx->height, AV_PIX_FMT_RGB565LE, 16);
                    LogI(TAG, DEBUG, "play_media img_covert begin");
                    img_covert(&pict, AV_PIX_FMT_RGB565LE, (AVPicture *)m_Frame, vcodec_ctx->pix_fmt, vcodec_ctx->width, vcodec_ctx->height);
                    LogI(TAG, DEBUG, "play_media renderSurface begin");
                    renderSurface(pict.data[0]);
                    av_freep(&pict.data[0]);
                }
                av_packet_unref(m_Packet);//释放 m_Packet 引用，防止内存泄漏
                LogI(TAG, DEBUG, "video_thread av_init_packet");
                av_init_packet(m_Packet);
            }
        
            LogI(TAG, DEBUG, "video_thread usleep");
            usleep(10000);
        }
        LogI(TAG, DEBUG, "play_media avcodec_open2 w/h: %d/%d", vcodec_ctx->width, vcodec_ctx->height);
    }


    failure:

    //11.释放资源，解码完成
    if(m_Frame != NULL) {
        av_frame_free(&m_Frame);
        m_Frame = NULL;
    }

    if(m_Packet != NULL) {
        av_packet_free(&m_Packet);
        m_Packet = NULL;
    }

    if(vcodec_ctx) {
        avcodec_close(vcodec_ctx);
        avcodec_free_context(&vcodec_ctx);
        vcodec_ctx = NULL;
        vcodec = NULL;
    }

    if(fmt_ctx)
    {
        avformat_close_input(&fmt_ctx);
        avformat_free_context(fmt_ctx);
    }
    avformat_network_deinit();
    LogI(TAG, DEBUG, "play_media end");
    return 0;
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
