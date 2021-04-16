// #include <jni.h>

#include "logger.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"
#include "player.h"


#include <pthread.h>

#include "libavutil/pixfmt.h"

#define TAG "ffmpeg_videoDecode"
#define DEBUG true

#define TEST_URL "/sdcard/test.mp4"
AVCodecContext *acodec_ctx;
AVCodecContext *vcodec_ctx;
PacketQueue video_queue;
/*
AVStream *vstream;
AVStream *astream;
AVCodec *vcodec;
AVCodec *acodec;
*/

void* video_thread(void *argv);
void video_render(AVCodecContext *vcodec_ctx, AVFrame *pFrame, AVPacket *pkt);
void packet_queue_init(PacketQueue *q) {
	memset(q, 0, sizeof(PacketQueue));
	pthread_mutex_init(q->mutex, NULL);
}

int packet_queue_put(PacketQueue *q, AVPacket *pkt) {
	AVPacketList *pkt1;

	if ((NULL == pkt) || (NULL == q)) {
		av_log(NULL, AV_LOG_ERROR,
				"packet_queue_put failure, q or pkt is NULL. \n");
		return -1;
	}

	if (av_dup_packet(pkt) < 0) {
		av_log(NULL, AV_LOG_ERROR, "packet_queue_put av_dup_packet failure.\n");
		return -1;
	}

	pkt1 = (AVPacketList*) av_malloc(sizeof(AVPacketList));
	if (!pkt1) {
		av_log(NULL, AV_LOG_ERROR, "packet_queue_put av_malloc failure.\n");
		return -1;
	}

	pkt1->pkt = *pkt;
	pkt1->next = NULL;

	pthread_mutex_lock(q->mutex);

	if (!q->last_pkt) {
		q->first_pkt = pkt1;
	} else {
		q->last_pkt->next = pkt1;
	}

	q->last_pkt = pkt1;
	q->nb_packets++;
	q->size += pkt1->pkt.size;

	pthread_mutex_unlock(q->mutex);

	return 0;
}

int packet_queue_get(PacketQueue *q, AVPacket *pkt) {
	AVPacketList *pkt1;
	int ret;

	pthread_mutex_lock(q->mutex);

	pkt1 = q->first_pkt;

	if (pkt1) {
		q->first_pkt = pkt1->next;

		if (!q->first_pkt) {
			q->last_pkt = NULL;
		}

		q->nb_packets--;
		q->size -= pkt1->pkt.size;
		*pkt = pkt1->pkt;
		av_free(pkt1);
		ret = 1;
	} else {
		ret = 0;
	}

	pthread_mutex_unlock(q->mutex);

	return ret;
}

int packet_queue_size(PacketQueue *q) {
	return q->size;
}

void open_media(void *argv)
{

    const char url[128] = "/sdcard/test.mp4";

    LogI(TAG, DEBUG, "open_media begin");
    int i = 0;
    int err = 0;
    int framecnt = 0;

    AVFormatContext *fmt_ctx = NULL;
    AVDictionaryEntry *dict = NULL;
    AVPacket pkt;
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

    if(-1 != video_stream_index)
    {
        pthread_create(&thread, NULL, video_thread, NULL);
    }
    LogI(TAG, DEBUG, "open_media avcodec_send_packet err");
    while (av_read_frame(fmt_ctx, &pkt) >= 0)
    {
        if (pkt.stream_index == video_stream_index) {
            packet_queue_put(&video_queue, &pkt);
        } else {
            av_free_packet(&pkt);
        }
    }
    usleep(1000);

    if(pFrame != NULL) {
        av_frame_free(&pFrame);
        pFrame = NULL;
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
        if (packet_queue_get(&video_queue, packet) <= 0)
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
            LogI(TAG, DEBUG, "open_media renderSurface end");
            //av_free(&pict.data[0]);
            LogI(TAG, DEBUG, "open_media av_free");
        }
        LogI(TAG, DEBUG, "open_media av_packet_unref");
        av_packet_unref(packet);
        LogI(TAG, DEBUG, "open_media av_init_packet");
        av_init_packet(packet);
        LogI(TAG, DEBUG, "open_media usleep");
        usleep(10000);
        LogI(TAG, DEBUG, "open_media usleep end");
    }
    LogI(TAG, DEBUG, "open_media av_free");
    av_free(pFrame);
    LogI(TAG, DEBUG, "open_media av_free end");
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