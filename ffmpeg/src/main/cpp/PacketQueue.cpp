//
// Created by Administrator on 2020/3/22.
//

#include "PacketQueue.h"

PacketQueue::PacketQueue(PlayStatus *playstatus) {
    this->playstatus = playstatus;
    pthread_mutex_init(&mutexPacket, NULL);
    pthread_cond_init(&condPacket, NULL);
}

PacketQueue::~PacketQueue() {
    pthread_mutex_destroy(&mutexPacket);
    pthread_cond_destroy(&condPacket);
}

int PacketQueue::putAvpacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);
    packetQueue.push(packet);
    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int PacketQueue::getAvpacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);
    while(playstatus != NULL && !playstatus->exit)
    {
        if(packetQueue.size() > 0)
        {
            AVPacket * avPacket = packetQueue.front();
            if(av_packet_ref(packet,avPacket) == 0){
                packetQueue.pop();
            }
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
//            logd("从队列里面取出一个AVpacket，还剩下 %d 个", packetQueue.size());
            break;
        }else {
            pthread_cond_wait(&condPacket,&mutexPacket);
        }
    }
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int PacketQueue::getQueueSize() {

    int size = 0;
    pthread_mutex_lock(&mutexPacket);
    size = packetQueue.size();
    pthread_mutex_unlock(&mutexPacket);
    return size;
}
