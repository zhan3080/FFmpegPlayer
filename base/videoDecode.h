// #include <jni.h>

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"
#include <pthread.h>

typedef struct GlobalContexts {
    AVCodecContext *acodec_ctx;
    AVCodecContext *vcodec_ctx;
    AVStream *vstream;
    AVStream *astream;
    AVCodec *vcodec;
    AVCodec *acodec;

    //PacketQueue video_queue;

    int quit;
    int pause;
}GlobalContext;

void open_media(void *argv);

