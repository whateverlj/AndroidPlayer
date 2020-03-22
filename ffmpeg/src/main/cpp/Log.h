//
// Created by Administrator on 2020/3/22.
//

#ifndef ANDROIDPLAYER_LOG_H
#define ANDROIDPLAYER_LOG_H

#include "android/log.h"
#define logd(FORMAT,...) __android_log_print(ANDROID_LOG_DEBUG,"FFMPEG",FORMAT,##__VA_ARGS__);
#define loge(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"FFMPEG",FORMAT,##__VA_ARGS__);


#endif //ANDROIDPLAYER_LOG_H
