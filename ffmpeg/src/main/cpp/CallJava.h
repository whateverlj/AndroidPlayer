//
// Created by Administrator on 2020/3/31.
//

#ifndef ANDROIDPLAYER_CALLJAVA_H
#define ANDROIDPLAYER_CALLJAVA_H
extern "C"{
#include <cstdint>
#include "Log.h"
};
#include <jni.h>
class CallJava {

public:
    _JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jmethodID jmid_renderyuv;
    jobject jobj;

public:
    CallJava(_JavaVM *javaVM, JNIEnv *env, jobject *jbj);
    ~CallJava();

    void onCallRenderYUV(int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv);

};


#endif //ANDROIDPLAYER_CALLJAVA_H
