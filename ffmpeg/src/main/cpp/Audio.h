//
// Created by Administrator on 2020/3/22.
//

#ifndef ANDROIDPLAYER_AUDIO_H
#define ANDROIDPLAYER_AUDIO_H

extern "C"{
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};
#include "PlayStatus.h"
#include "PacketQueue.h"

class Audio {
public:
    int streamIndex = -1;
    int sampleRate = 0;
    AVCodecParameters *avCodecParameters = NULL;
    AVCodecContext * avCodecContext = NULL;
    PlayStatus * playstatus = NULL;
    PacketQueue *queue = NULL;

    pthread_t thread_play;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    int ret = 0;
    uint8_t *buffer = NULL;
    int data_size = 0;

    // 引擎接口
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;
    //混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    //pcm
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;
    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;
public:
    Audio(PlayStatus * playStatus,int sampleRate);
    ~Audio();

    void play();
    int reSampleAudio();
    void initOpenSL();
    uint getCurrentSampleRateForOpensles(int sample_rate);
};


#endif //ANDROIDPLAYER_AUDIO_H
