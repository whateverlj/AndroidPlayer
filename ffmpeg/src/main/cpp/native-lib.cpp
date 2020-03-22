#include <jni.h>
#include <string>
#include "Log.h"
#include "FFmpegHandle.h"

FFmpegHandle * ffmpegHandle = NULL;
PlayStatus *playstatus = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpeg_FFmpeg_initAvCodec(JNIEnv *env, jclass clazz, jstring source) {
        loge("解码器初始化");
        const  char * cSource = env->GetStringUTFChars(source, NULL);
        if(ffmpegHandle == NULL){
                playstatus = new PlayStatus();
                ffmpegHandle = new FFmpegHandle(cSource,playstatus);
                ffmpegHandle->initAvCodec();
        }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpeg_FFmpeg_startPlay(JNIEnv *env, jclass clazz) {
    // TODO: implement start()
       loge("开始播放");
        if(ffmpegHandle != NULL)
        {
                ffmpegHandle->startPlay();
        }
}