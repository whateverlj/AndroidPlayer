//
// Created by Administrator on 2020/3/22.
//
#include "FFmpegHandle.h"

FFmpegHandle::FFmpegHandle(const char *cSource, PlayStatus *playstatus,CallJava *callJava) {
    this->cSource = cSource;
    this->playstatus = playstatus;
    this->callJava = callJava;
}

FFmpegHandle::~FFmpegHandle() {

}

void *initAcodesCallBack(void *data) {
    FFmpegHandle *ffmpegHandle = (FFmpegHandle *) data;
    ffmpegHandle->prepare();
    pthread_exit(&ffmpegHandle->initAcodecThread);
}

/**
 * 初始化解码器
 */
void FFmpegHandle::initAvCodec() {
    pthread_create(&initAcodecThread, NULL, initAcodesCallBack, this);
}

void FFmpegHandle::prepare() {
    // 注册解码器
    av_register_all();
    // 初始化网络协议
    avformat_network_init();

    // 分配内存
    avFormatContext = avformat_alloc_context();
    //打开流
    if (avformat_open_input(&avFormatContext, cSource, NULL, NULL) != 0) {
        loge("打开音频流失败");
        return;
    };
    //找到流
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        loge("找到音频流失败");
        return;
    };
    // 读取音频流和视频流
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        // 如果是音频流
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (audio == NULL) {
                audio = new Audio(playstatus, avFormatContext->streams[i]->codecpar->sample_rate);
                audio->streamIndex = i;
                audio->avCodecParameters = avFormatContext->streams[i]->codecpar;
                audio->time_base = avFormatContext->streams[i]->time_base;
            }
        };
        //如果是视频流
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video == NULL) {
                video = new Video(playstatus,callJava);
                video->streamIndex = i;
                video->avCodecParameters = avFormatContext->streams[i]->codecpar;
                video->time_base = avFormatContext->streams[i]->time_base;
                int num = avFormatContext->streams[i]->avg_frame_rate.num;
                int den = avFormatContext->streams[i]->avg_frame_rate.den;
                if(num != 0 && den != 0)
                {
                    int fps = num / den;//[25 / 1]
                    video->defaultDelayTime = 1.0 / fps;
                }
            }
        }
    }
    // 打開解码器上下文
    if (audio != NULL) {
        avcodecContextInit(audio->avCodecParameters, &(audio->avCodecContext));
    }
    if (video != NULL) {
        avcodecContextInit(video->avCodecParameters, &(video->avCodecContext));
    }
    loge("初始化解码器成功");
}


/**
 * 解码并且播放
 */
void FFmpegHandle::startPlay() {
    loge("开始播放");
    if (audio == NULL) {
        loge("audio IS NULL");
        return;
    }
    if (video == NULL) {
        loge("video IS NULL");
        return;
    }
    video->audio = audio;
    // 播放
    audio->play();
    video->play();
    // 解码
    while (playstatus != NULL && !playstatus->exit) {
        AVPacket *avPacket = av_packet_alloc();
        // 读取帧数据
        if (av_read_frame(avFormatContext, avPacket) == 0) {
            // 解码
            if (avPacket->stream_index == audio->streamIndex) {
                audio->packetQueue->putAvpacket(avPacket);
            } else if (avPacket->stream_index == video->streamIndex) {
                video->packetQueue->putAvpacket(avPacket);
            } else {
                av_packet_free(&avPacket);
                av_free(avPacket);
            }
        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            av_packet_free(&avPacket);
            av_free(avPacket);
            while (playstatus != NULL && !playstatus->exit) {
                if (audio->packetQueue->getQueueSize() > 0) {
                    continue;
                } else {
                    playstatus->exit = true;
                    break;
                }
            }
        }
    }
    loge("解码結束");

}

/**
 * 解碼器上下文初始化
 */
void FFmpegHandle::avcodecContextInit(AVCodecParameters *avCodecParameters,
                                      AVCodecContext **avCodecContext) {
    // 获取解码器
    AVCodec *avCodec = avcodec_find_decoder(avCodecParameters->codec_id);
    if (!avCodec) {
        loge("获取解码器失败");
        return;
    }
    // 解码器分配内存并且赋值给 解码器上下文
    *avCodecContext = avcodec_alloc_context3(avCodec);
    if (!audio->avCodecContext) {
        loge("解码器上下文赋值失败");
        return;
    }
    // 解码器参数传递给解码器上下文
    if (avcodec_parameters_to_context(*avCodecContext, avCodecParameters) < 0) {
        loge("解码器参数传递给解码器上下文失败");
        return;
    }
    // 打开解码器
    if (avcodec_open2(*avCodecContext, avCodec, NULL)) {
        loge("打开解码器失败");
        return;
    };
}
