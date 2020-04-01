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
    PacketQueue packetQueue = NULL;
    CallJava *callJava = NULL;

    Video *video = NULL;

public:
    FFmpegHandle(const char *cSource,PlayStatus * playStatus,CallJava *callJava );
    ~FFmpegHandle();

public:
    void initAvCodec();
    void prepare();
    void startPlay();
    void avcodecContextInit(AVCodecParameters *avCodecParameters,AVCodecContext ** avCodecContext );


};


#endif //ANDROIDPLAYER_FFMPEGHANDLE_H
