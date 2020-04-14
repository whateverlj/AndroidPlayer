//
// Created by Administrator on 2020/3/22.
//

#ifndef ANDROIDPLAYER_FFMPEGHANDLE_H
#define ANDROIDPLAYER_FFMPEGHANDLE_H

#include <string>
#include "Log.h"
#include "Audio.h"
#include "pthread.h"
#include "PacketQueue.h"
#include "Video.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

class FFmpegHandle {

public:
    pthread_t  initAcodecThread;
    const char *cSource = NULL;
    AVFormatContext *avFormatContext = NULL;
    Audio *audio = NULL;
    PlayStatus *playstatus = NULL;
    CallJava *callJava = NULL;
    pthread_mutex_t init_mutex;
    Video *video = NULL;
    bool isExit = false;
    int duration = 0;
    pthread_mutex_t seek_mutex;
    bool supportMediacodec = false;
    const AVBitStreamFilter *bsFilter = NULL;
public:
    FFmpegHandle(const char *cSource,PlayStatus * playStatus,CallJava *callJava );
    ~FFmpegHandle();

public:
    void initAvCodec();
    void prepare();
    void startPlay();
    void avcodecContextInit(AVCodecParameters *avCodecParameters,AVCodecContext ** avCodecContext );
    void pausePlay();
    void resumePlay();
    void stopPlay();
    void seek(int64_t secds);
};


#endif //ANDROIDPLAYER_FFMPEGHANDLE_H
