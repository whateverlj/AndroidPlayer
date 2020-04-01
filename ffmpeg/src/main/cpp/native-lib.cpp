#include <jni.h>
#include <string>
#include "Log.h"
#include "FFmpegHandle.h"

FFmpegHandle * ffmpegHandle = NULL;
PlayStatus *playstatus = NULL;
CallJava *callJava = NULL;
_JavaVM* javaVM = NULL;

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

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpeg_FFmpeg_startPlay(JNIEnv *env, jobject jbj) {
    // TODO: implement start()
        if(ffmpegHandle != NULL)
        {
                ffmpegHandle->startPlay();
        }
}