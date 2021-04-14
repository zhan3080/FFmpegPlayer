// #include <jni.h>

#include "logger.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"


#include <pthread.h>

#include "libavutil/pixfmt.h"

#define TAG "ffmpeg_videoDecode"
#define DEBUG true

#define TEST_URL "/sdcard/test.mp4"
AVCodecContext *acodec_ctx;
AVCodecContext *vcodec_ctx;
void* video_thread(void *argv);
void video_render(AVCodecContext *vcodec_ctx, AVFrame *pFrame, AVPacket *pkt);

void open_media(void *argv)
{

    const char url[128] = "/sdcard/test.mp4";

    LogI(TAG, DEBUG, "open_media begin");
    int i = 0;
    int err = 0;
    int framecnt = 0;
    AVFormatContext *fmt_ctx = NULL;
    AVDictionaryEntry *dict = NULL;
    AVPacket *pkt;
    AVFrame *pFrame;
    int video_stream_index = -1;
    pthread_t thread;

    avfilter_register_all();
    av_register_all();
    avformat_network_init();

    fmt_ctx = avformat_alloc_context();

    err = avformat_open_input(&fmt_ctx, TEST_URL, NULL, NULL);
    if(err < 0)
    {
        LogE(TAG, DEBUG, "open_media avformat_open_input fail err:" + err);
        return;
    }
    LogI(TAG, DEBUG, "open_media avformat_find_stream_info");
    if((err == avformat_find_stream_info(fmt_ctx,NULL)) < 0)
    {
        LogE(TAG, DEBUG, "open_media avformat_find_stream_info fail err:" + err);
        return;
    }

    for(i = 0; i < fmt_ctx->nb_streams; i++)
    {
        LogE(TAG, DEBUG, "open_media codec_type %d",fmt_ctx->streams[i]->codec->codec_type);
        if(fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_stream_index = i;
        }
    }
    LogE(TAG, DEBUG, "open_media video_stream_index %d",video_stream_index);

    if(-1 != video_stream_index)
    {
        vcodec_ctx = fmt_ctx->streams[video_stream_index]->codec;

        AVCodec *vcodec = avcodec_find_decoder(vcodec_ctx->codec_id);
        if(NULL == vcodec)
        {
            LogE(TAG, DEBUG, "open_media avcodec_find_decoder fail");
            return;
        }

        if(avcodec_open2(vcodec_ctx,vcodec, NULL) < 0)
        {
            LogE(TAG, DEBUG, "open_media avcodec_open2 fail");
        }

        if((vcodec_ctx->width > 0) && (vcodec_ctx->height > 0))
        {
            setBuffersGeometry(vcodec_ctx->width, vcodec_ctx->height);
        }
        LogI(TAG, DEBUG, "open_media avcodec_open2 w/h: %d/%d", vcodec_ctx->width, vcodec_ctx->height);
    }

    /*if(-1 != video_stream_index)
    {
        pthread_create(&thread, NULL, video_thread, NULL);
    }*/
    LogI(TAG, DEBUG, "open_media avcodec_send_packet err");
    while(av_read_frame(fmt_ctx, pkt) >=0)
    {
        LogI(TAG, DEBUG, "open_media avcodec_send_packet, stream_index=%d",pkt->stream_index);
        if(pkt->stream_index == video_stream_index)
        {
            LogE(TAG, DEBUG, "open_media avcodec_send_packet ");
            if(err = avcodec_send_packet(fmt_ctx, pkt) != 0)
            {
                LogE(TAG, DEBUG, "open_media avcodec_send_packet fail err=%d",err);
                return;
            }
            while (avcodec_receive_frame(fmt_ctx, pFrame) == 0)
            {
                video_render(fmt_ctx, pFrame, pkt);
                usleep(10000);
            }
            av_packet_unref(pkt);
        }
    }

    if(pFrame != NULL) {
        av_frame_free(&pFrame);
        pFrame = NULL;
    }

    if(pkt != NULL) {
        av_packet_free(&pkt);
        pkt = NULL;
    }

    if(fmt_ctx)
    {
        avformat_close_input(&fmt_ctx);
        avformat_free_context(fmt_ctx);
    }
    avformat_network_deinit();

    return;
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

void* video_thread(void *argv)
{
    AVPacket pkt1;
    AVPacket *packet = &pkt1;
    int frameFinished;
    AVFrame *pFrame;
    double pts;

    pFrame = av_frame_alloc();
    for(;;)
    {
        if (avcodec_receive_frame(vcodec_ctx, pFrame) <= 0)
        {
            continue;
        }

        LogI(TAG, DEBUG, "open_media avcodec_decode_video2");
        avcodec_decode_video2(vcodec_ctx, pFrame, &frameFinished, packet);

        LogI(TAG, DEBUG, "open_media frameFinished:" + frameFinished);
        if(frameFinished)
        {
            AVPicture pict;
            uint8_t *dst_data[4];
            int dst_linesize[4];

            LogI(TAG, DEBUG, "open_media av_image_alloc");
            av_image_alloc(pict.data, pict.linesize, vcodec_ctx->width, vcodec_ctx->height, AV_PIX_FMT_RGB565LE, 16);
            LogI(TAG, DEBUG, "open_media img_covert begin");
            img_covert(&pict, AV_PIX_FMT_RGB565LE, (AVPicture *)pFrame, vcodec_ctx->pix_fmt, vcodec_ctx->width, vcodec_ctx->height);
            LogI(TAG, DEBUG, "open_media renderSurface begin");
            renderSurface(pict.data[0]);
            av_free(&pict.data[0]);
        }
        av_packet_unref(packet);
        av_init_packet(packet);
        usleep(10000);
    }

    av_free(pFrame);
}

void video_render(AVCodecContext *vcodec_ctx, AVFrame *pFrame, AVPacket *pkt)
{
    int frameFinished;
    LogI(TAG, DEBUG, "video_render avcodec_decode_video2");
    avcodec_decode_video2(vcodec_ctx, pFrame, &frameFinished, pkt);

    LogI(TAG, DEBUG, "video_render frameFinished:" + frameFinished);
    if(frameFinished)
    {
        AVPicture pict;
        uint8_t *dst_data[4];
        int dst_linesize[4];

        LogI(TAG, DEBUG, "video_render av_image_alloc");
        av_image_alloc(pict.data, pict.linesize, vcodec_ctx->width, vcodec_ctx->height, AV_PIX_FMT_RGB565LE, 16);
        LogI(TAG, DEBUG, "video_render img_covert begin");
        img_covert(&pict, AV_PIX_FMT_RGB565LE, (AVPicture *)pFrame, vcodec_ctx->pix_fmt, vcodec_ctx->width, vcodec_ctx->height);
        LogI(TAG, DEBUG, "video_render renderSurface begin");
        renderSurface(pict.data[0]);
        av_free(&pict.data[0]);
    }
}