//
// Created by Administrator on 2020/3/22.
//

#ifndef ANDROIDPLAYER_PACKETQUEUE_H
#define ANDROIDPLAYER_PACKETQUEUE_H
#include "PlayStatus.h"

#include "pthread.h"
#include "queue"
#include "Log.h"

extern "C"{
#include "include/libavcodec/avcodec.h"
};
class PacketQueue {

public:
    std::queue<AVPacket *> packetQueue;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
    PlayStatus *playstatus = NULL;
public:
    PacketQueue(PlayStatus *playstatus);
    ~PacketQueue();

    int putAvpacket(AVPacket *packet);
    int getAvpacket(AVPacket *packet);
    int getQueueSize();
};


#endif //ANDROIDPLAYER_PACKETQUEUE_H
