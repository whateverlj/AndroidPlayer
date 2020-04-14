//
// Created by Administrator on 2020/3/29.
//

#ifndef ANDROIDPLAYER_VIDEO_H
#define ANDROIDPLAYER_VIDEO_H

extern "C"{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
};

#include "PlayStatus.h"
#include "PacketQueue.h"
#include "CallJava.h"
#include "Audio.h"

#define CODEC_YUV 0
#define CODEC_MEDIACODEC 1
class Video {

public:
    int streamIndex = -1;
    PlayStatus* playstatus;
    AVCodecParameters *avCodecParameters = NULL;
    AVCodecContext * avCodecContext = NULL;
    PacketQueue *packetQueue = NULL;
    pthread_t  play_Pthread ;
    CallJava *callJava = NULL;
    double clock = 0;
    AVRational time_base;
    Audio *audio = NULL;
    double delayTime = 0;
    double defaultDelayTime = 0.04;
    pthread_mutex_t mutex ;
    AVBSFContext *abs_ctx = NULL;
    int codecType = CODEC_YUV;
public:
    Video(PlayStatus *plastatus, CallJava *callJava);
    ~Video();

    void play();
    double getFrameDiffTime(AVFrame *avFrame, AVPacket *avPacket);
    double getDelayTime(double distance);
    void release();

};


#endif //ANDROIDPLAYER_VIDEO_H
