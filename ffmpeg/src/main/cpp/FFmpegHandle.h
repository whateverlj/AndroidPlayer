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

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

class FFmpegHandle {

public:
    pthread_t  initAcodecThread = NULL;

    const char *cSource = NULL;
    AVFormatContext *avFormatContext = NULL;
    Audio *audio = NULL;
    PlayStatus *playstatus = NULL;
    PacketQueue packetQueue = NULL;

public:
    FFmpegHandle(const char *cSource,PlayStatus * playStatus);
    ~FFmpegHandle();

public:
    void initAvCodec();
    void prepare();
    void startPlay();


};


#endif //ANDROIDPLAYER_FFMPEGHANDLE_H
