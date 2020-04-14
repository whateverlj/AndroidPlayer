//
// Created by Administrator on 2020/3/31.
//

#ifndef ANDROIDPLAYER_CALLJAVA_H
#define ANDROIDPLAYER_CALLJAVA_H

#define MAIN_THREAD 0
#define CHILD_THREAD 1

extern "C"{
#include <cstdint>
#include "Log.h"
};
#include <jni.h>
class CallJava {

public:
    JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;
    jmethodID jmid_renderyuv;
    jmethodID jmid_parpared;
    jmethodID jmid_timeinfo;
    jmethodID jmid_complete;
    jmethodID jmid_supportvideo;
    jmethodID jmid_initmediacodec;
    jmethodID jmid_decodeavpacket;
public:
    CallJava(JavaVM *javaVM, JNIEnv *env, jobject *jbj);
    ~CallJava();
    void onCallParpared(int type);
    void onCallRenderYUV(int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv);
    void onCallTimeInfo(int type, int curr, int total);
    void onCallComplete(int type);
    bool onCallIsSupportVideo(const char *ffcodecname);
    void onCallInitMediacodec(const char *mime, int width, int height, int csd0_size, int csd1_size, uint8_t *csd_0, uint8_t *csd_1);
    void onCallDecodeAVPacket(int datasize,uint8_t *packetdata);
};


#endif //ANDROIDPLAYER_CALLJAVA_H
