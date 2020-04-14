#include <jni.h>
#include <string>
#include "Log.h"
#include "FFmpegHandle.h"

FFmpegHandle * ffmpegHandle = NULL;
PlayStatus *playstatus = NULL;
CallJava *callJava = NULL;
_JavaVM* javaVM = NULL;

bool isExit = false;
pthread_t thread_start;

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(_JavaVM *vm, void *reserved)
{
    jint result = -1;
    javaVM = vm;
    JNIEnv *env;
    if(vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        return result;
    }
    return JNI_VERSION_1_4;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpeg_FFmpeg_initAvCodec(JNIEnv *env, jobject obj, jstring source) {
        loge("解码器初始化");
        const  char * cSource = env->GetStringUTFChars(source, NULL);
        if(ffmpegHandle == NULL){
                playstatus = new PlayStatus();
                if(callJava == NULL)
                {
                    callJava = new CallJava(javaVM, env, &obj);
                }
                ffmpegHandle = new FFmpegHandle(cSource,playstatus,callJava);
                ffmpegHandle->initAvCodec();
        }
}

void *startCallBack(void *data)
{
    FFmpegHandle *fFmpeg = (FFmpegHandle *) data;
    fFmpeg->startPlay();
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpeg_FFmpeg_startPlay(JNIEnv *env, jobject jbj) {
    // TODO: implement start()
        if(ffmpegHandle != NULL)
        {
            pthread_create(&thread_start, NULL,startCallBack,ffmpegHandle);
        }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpeg_FFmpeg_pausePlay(JNIEnv *env, jobject thiz) {
    // TODO: implement pausePlay()
    if(ffmpegHandle != NULL)
    {
        ffmpegHandle->pausePlay();
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpeg_FFmpeg_resumePlay(JNIEnv *env, jobject thiz) {
    // TODO: implement resumePlay()
    if(ffmpegHandle != NULL)
    {
        ffmpegHandle->resumePlay();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpeg_FFmpeg_stopPlay(JNIEnv *env, jobject thiz) {
    // TODO: implement stopPlay()
    if(isExit)
    {
        return;
    }
    isExit = true;
    if(ffmpegHandle != NULL)
    {
        loge("释放 ffmpegHandle");
        ffmpegHandle->stopPlay();
        loge("释放 ffmpegHandle end");
        pthread_join(thread_start, NULL);
        delete(ffmpegHandle);
        ffmpegHandle = NULL;
        if(callJava != NULL)
        {
            delete(callJava);
            callJava = NULL;
        }
        if(playstatus != NULL)
        {
            delete(playstatus);
            playstatus = NULL;
        }
    }
    loge("释放  end");
    isExit = false;
}



extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpeg_FFmpeg_seekPlay(JNIEnv *env, jobject thiz, jint secds) {
    // TODO: implement seekPlay()

    if(ffmpegHandle != NULL)
    {
        ffmpegHandle->seek(secds);
    }

}