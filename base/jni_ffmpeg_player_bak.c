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
#include "videoDecode.h"

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
jint decode(JNIEnv* env, jobject thiz, jstring input_jstr, jstring output_jstr);
jint stream(JNIEnv* env, jobject thiz, jstring input_jstr, jstring output_jstr);
jint SetSurface(JNIEnv* env, jobject thiz, jobject jsurface);
jint pausePlayer(JNIEnv* env, jobject thiz);
jint resumePlayer(JNIEnv* env, jobject thiz);
jint stopPlayer(JNIEnv* env, jobject thiz);

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

jint decode(JNIEnv* env, jobject thiz, jstring input_jstr, jstring output_jstr)
{
    AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame,*pFrameYUV;
	uint8_t *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;
	FILE *fp_yuv;
	int frame_cnt;
	clock_t time_start, time_finish;
	double  time_duration = 0.0;
 
	char input_str[500]={0};
	char output_str[500]={0};
	char info[1000]={0};
	sprintf(input_str,"%s",(*env)->GetStringUTFChars(env,input_jstr, NULL));
	sprintf(output_str,"%s",(*env)->GetStringUTFChars(env,output_jstr, NULL));
	LogI(TAG,DEBUG,"input_str:%s", input_str);
	LogI(TAG,DEBUG,"output_str:%s", output_str);
 
	//FFmpeg av_log() callback
//   av_log_set_callback(custom_log);
	
	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
 
	if(avformat_open_input(&pFormatCtx,input_str,NULL,NULL)!=0){
        LogE(TAG,DEBUG,"Couldn't open input stream");
		return -1;
	}
	if(avformat_find_stream_info(pFormatCtx,NULL)<0){
		LogE(TAG,DEBUG,"Couldn't find stream information.\n");
		return -1;
	}
	videoindex=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++) 
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
			videoindex=i;
			break;
		}
	if(videoindex==-1){
		LogE(TAG,DEBUG,"Couldn't find a video stream.\n");
		return -1;
	}
	pCodecCtx=pFormatCtx->streams[videoindex]->codec;
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL){
		LogE(TAG,DEBUG,"Couldn't find Codec.\n");
		return -1;
	}
	if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
		LogE(TAG,DEBUG,"Couldn't open codec.\n");
		return -1;
	}
	
	pFrame=av_frame_alloc();
	pFrameYUV=av_frame_alloc();
	out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  pCodecCtx->width, pCodecCtx->height,1));
	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
		AV_PIX_FMT_YUV420P,pCodecCtx->width, pCodecCtx->height,1);
	
	
	packet=(AVPacket *)av_malloc(sizeof(AVPacket));
 
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
	pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 
 
  
  sprintf(info,   "[Input     ]%s\n", input_str);
  sprintf(info, "%s[Output    ]%s\n",info,output_str);
  sprintf(info, "%s[Format    ]%s\n",info, pFormatCtx->iformat->name);
  sprintf(info, "%s[Codec     ]%s\n",info, pCodecCtx->codec->name);
  sprintf(info, "%s[Resolution]%dx%d\n",info, pCodecCtx->width,pCodecCtx->height);
 
 
  fp_yuv=fopen(output_str,"wb+");
  if(fp_yuv==NULL){
		LogE(TAG,DEBUG,"Cannot open output file.\n");
		return -1;
	}
	
	frame_cnt=0;
	time_start = clock();
	
	while(av_read_frame(pFormatCtx, packet)>=0){
		if(packet->stream_index==videoindex){
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			if(ret < 0){
				LogE(TAG,DEBUG,"Decode Error.\n");
				return -1;
			}
			if(got_picture){
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, 
					pFrameYUV->data, pFrameYUV->linesize);
				
				y_size=pCodecCtx->width*pCodecCtx->height;  
				fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y 
				fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
				fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
				//Output info
				char pictype_str[10]={0};
				switch(pFrame->pict_type){
					case AV_PICTURE_TYPE_I:sprintf(pictype_str,"I");break;
				  case AV_PICTURE_TYPE_P:sprintf(pictype_str,"P");break;
					case AV_PICTURE_TYPE_B:sprintf(pictype_str,"B");break;
					default:sprintf(pictype_str,"Other");break;
				}
				LogI(TAG,DEBUG,"Frame Index: %5d. Type:%s",frame_cnt,pictype_str);
				frame_cnt++;
			}
		}
		av_free_packet(packet);
	}
	//flush decoder
	//FIX: Flush Frames remained in Codec
	while (1) {
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
		if (ret < 0)
			break;
		if (!got_picture)
			break;
		sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, 
			pFrameYUV->data, pFrameYUV->linesize);
		int y_size=pCodecCtx->width*pCodecCtx->height;  
		fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y 
		fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
		fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
		//Output info
		char pictype_str[10]={0};
		switch(pFrame->pict_type){
			case AV_PICTURE_TYPE_I:sprintf(pictype_str,"I");break;
		  case AV_PICTURE_TYPE_P:sprintf(pictype_str,"P");break;
			case AV_PICTURE_TYPE_B:sprintf(pictype_str,"B");break;
			default:sprintf(pictype_str,"Other");break;
		}
		LogI(TAG,DEBUG,"Frame Index: %5d. Type:%s",frame_cnt,pictype_str);
		frame_cnt++;
	}
	time_finish = clock(); 
	time_duration=(double)(time_finish - time_start);
	
	sprintf(info, "%s[Time      ]%fms\n",info,time_duration);
	sprintf(info, "%s[Count     ]%d\n",info,frame_cnt);
 
	sws_freeContext(img_convert_ctx);
 
  fclose(fp_yuv);
 
	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
    
    return 0;
}


jint stream(JNIEnv* env, jobject thiz, jstring input_jstr, jstring output_jstr)
{
	AVOutputFormat *ofmt = NULL;
	AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
	AVPacket pkt;
 
	int ret, i;
	char input_str[500]={0};
	char output_str[500]={0};
	char info[1000]={0};
	sprintf(input_str,"%s",(*env)->GetStringUTFChars(env,input_jstr, NULL));
	sprintf(output_str,"%s",(*env)->GetStringUTFChars(env,output_jstr, NULL));
 
	//input_str  = "cuc_ieschool.flv";
	//output_str = "rtmp://localhost/publishlive/livestream";
	//output_str = "rtp://233.233.233.233:6666";
 
	//FFmpeg av_log() callback
	// av_log_set_callback(custom_log);
 
	av_register_all();
	//Network
	avformat_network_init();
 
	//Input
	if ((ret = avformat_open_input(&ifmt_ctx, input_str, 0, 0)) < 0) {
		LogE(TAG,DEBUG, "Could not open input file.");
		goto end;
	}
	if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
		LogE(TAG,DEBUG, "Failed to retrieve input stream information");
		goto end;
	}
 
	int videoindex=-1;
	for(i=0; i<ifmt_ctx->nb_streams; i++) 
		if(ifmt_ctx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
			videoindex=i;
			break;
		}
	//Output
	int ret1 = avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv",output_str); //RTMP
	//avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", output_str);//UDP
	LogE(TAG,DEBUG, "avformat_alloc_output_context2 ret:%d\n",ret1);
	LogI(TAG,DEBUG, "iavformat_alloc_output_context2 ret:%d\n",ret1);
 
	if (!ofmt_ctx) {
		LogE(TAG,DEBUG, "Could not create output context\n");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	ofmt = ofmt_ctx->oformat;
	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		//Create output AVStream according to input AVStream
		AVStream *in_stream = ifmt_ctx->streams[i];
		AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
		if (!out_stream) {
			LogE(TAG,DEBUG, "Failed allocating output stream\n");
			ret = AVERROR_UNKNOWN;
			goto end;
		}
		//Copy the settings of AVCodecContext
		ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
		if (ret < 0) {
			LogE(TAG,DEBUG, "Failed to copy context from input to output stream codec context\n");
			goto end;
		}
		out_stream->codec->codec_tag = 0;
		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}
 
	//Open output URL
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&ofmt_ctx->pb, output_str, AVIO_FLAG_WRITE);
		if (ret < 0) {
			LogE(TAG,DEBUG, "Could not open output URL '%s'", output_str);
			goto end;
		}
	}
	//Write file header
	ret = avformat_write_header(ofmt_ctx, NULL);
	if (ret < 0) {
		LogE(TAG,DEBUG, "Error occurred when opening output URL\n");
		goto end;
	}
 
	int frame_index=0;
 
	int64_t start_time=av_gettime();
	while (1) {
		AVStream *in_stream, *out_stream;
		//Get an AVPacket
		ret = av_read_frame(ifmt_ctx, &pkt);
		if (ret < 0)
			break;
		//FIX：No PTS (Example: Raw H.264)
		//Simple Write PTS
		if(pkt.pts==AV_NOPTS_VALUE){
			//Write PTS
			AVRational time_base1=ifmt_ctx->streams[videoindex]->time_base;
			//Duration between 2 frames (us)
			int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(ifmt_ctx->streams[videoindex]->r_frame_rate);
			//Parameters
			pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
			pkt.dts=pkt.pts;
			pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
		}
		//Important:Delay
		if(pkt.stream_index==videoindex){
			AVRational time_base=ifmt_ctx->streams[videoindex]->time_base;
			AVRational time_base_q={1,AV_TIME_BASE};
			int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
			int64_t now_time = av_gettime() - start_time;
			if (pts_time > now_time)
				av_usleep(pts_time - now_time);
 
		}
 
		in_stream  = ifmt_ctx->streams[pkt.stream_index];
		out_stream = ofmt_ctx->streams[pkt.stream_index];
		/* copy packet */
		//Convert PTS/DTS
		pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
		pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
		pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
		pkt.pos = -1;
		//Print to Screen
		if(pkt.stream_index==videoindex){
			LogE(TAG,DEBUG, "Send %8d video frames to output URL\n",frame_index);
			frame_index++;
		}
		//ret = av_write_frame(ofmt_ctx, &pkt);
		ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
 
		if (ret < 0) {
			LogE(TAG,DEBUG, "Error muxing packet\n");
			break;
		}
		av_free_packet(&pkt);
		
	}
	//Write file trailer
	av_write_trailer(ofmt_ctx);
end:
	avformat_close_input(&ifmt_ctx);
	/* close output */
	if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
		avio_close(ofmt_ctx->pb);
	avformat_free_context(ofmt_ctx);
	if (ret < 0 && ret != AVERROR_EOF) {
		LogE(TAG,DEBUG, "Error occurred.\n");
		return -1;
	}
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
    if(mANativeWindow == NULL){
        LogE(TAG,DEBUG, "mANativeWindow is ");
    }
    mANativeWindow = ANativeWindow_fromSurface(env,surface);
    LogE(TAG,DEBUG, "mANativeWindow is ok-- ");
    if(mANativeWindow == NULL){
        LogE(TAG,DEBUG, "mANativeWindow is null");
    }
    pthread_t thread1;
    LogE(TAG,DEBUG, "pthread_create open_media");
    pthread_create(&thread1, NULL, open_media, NULL);
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


