LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libavcodec
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/libavcodec.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libavfilter
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/libavfilter.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libavformat
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/libavformat.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libavutil
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/libavutil.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libswresample
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/libswresample.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libswscale
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/libswscale.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := ffplayer


LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/logger.h 



LOCAL_LDLIBS := -llog

#sourcecode for ffplayer
LOCAL_SRC_FILES := jni_ffmpeg_player.c

LOCAL_STATIC_LIBRARIES := \
	libavcodec \
	libavfilter \
	libavformat \
  	libavutil \
	libswresample \
  	libswscale


LOCAL_LDFLAGS += -Wl,--gc-sections


include $(BUILD_SHARED_LIBRARY)
