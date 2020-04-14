//
// Created by Administrator on 2020/3/22.
//
#include "FFmpegHandle.h"

FFmpegHandle::FFmpegHandle(const char *cSource, PlayStatus *playstatus,CallJava *callJava) {
    this->cSource = cSource;
    this->playstatus = playstatus;
    this->callJava = callJava;
    pthread_mutex_init(&init_mutex, NULL);
    isExit = false;
    pthread_mutex_init(&seek_mutex, NULL);
}

FFmpegHandle::~FFmpegHandle() {
    pthread_mutex_destroy(&init_mutex);
    pthread_mutex_destroy(&seek_mutex);
}

void *initAcodesCallBack(void *data) {
    FFmpegHandle *ffmpegHandle = (FFmpegHandle *) data;
    ffmpegHandle->prepare();
    return 0;
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
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    };
    //找到流
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        loge("找到音频流失败");
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    };
    // 读取音频流和视频流
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        // 如果是音频流
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (audio == NULL) {
                audio = new Audio(playstatus, avFormatContext->streams[i]->codecpar->sample_rate,callJava);
                audio->streamIndex = i;
                audio->avCodecParameters = avFormatContext->streams[i]->codecpar;
                audio->time_base = avFormatContext->streams[i]->time_base;
                audio->duration = avFormatContext->duration / AV_TIME_BASE;
                duration = audio->duration;
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
    if(callJava != NULL)
    {
        if(playstatus != NULL && !playstatus->exit)
        {
            callJava->onCallParpared(CHILD_THREAD);
        } else{
            isExit = true;
        }
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
    supportMediacodec = false;
    video->audio = audio;
    // 播放
    audio->play();
    video->play();
    const char* codecName = video->avCodecContext->codec->name;
    if(supportMediacodec = callJava->onCallIsSupportVideo(codecName))
    {
        loge("当前设备支持硬解码当前视频");
        if(strcasecmp(codecName, "h264") == 0)
        {
            bsFilter = av_bsf_get_by_name("h264_mp4toannexb");
        }
        else if(strcasecmp(codecName, "h265") == 0)
        {
            bsFilter = av_bsf_get_by_name("hevc_mp4toannexb");
        }
        if(bsFilter == NULL)
        {
            supportMediacodec = false;
            goto end;
        }
        if(av_bsf_alloc(bsFilter, &video->abs_ctx) != 0)
        {
            supportMediacodec = false;
            goto end;
        }
        if(avcodec_parameters_copy(video->abs_ctx->par_in, video->avCodecParameters) < 0)
        {
            supportMediacodec = false;
            av_bsf_free(&video->abs_ctx);
            video->abs_ctx = NULL;
            goto end;
        }
        if(av_bsf_init(video->abs_ctx) != 0)
        {
            supportMediacodec = false;
            av_bsf_free(&video->abs_ctx);
            video->abs_ctx = NULL;
            goto end;
        }
        video->abs_ctx->time_base_in = video->time_base;
    }

    end:
//    supportMediacodec = false;
    if(supportMediacodec)
    {
        video->codecType = CODEC_MEDIACODEC;
        video->callJava->onCallInitMediacodec(
                codecName,
                video->avCodecContext->width,
                video->avCodecContext->height,
                video->avCodecContext->extradata_size,
                video->avCodecContext->extradata_size,
                video->avCodecContext->extradata,
                video->avCodecContext->extradata
        );
    }
    // 解码
    while (playstatus != NULL && !playstatus->exit) {
        if(playstatus->seek)
        {
            av_usleep(1000 * 100);
            continue;
        }

        if(audio->packetQueue->getQueueSize() > 40)
        {
            av_usleep(1000 * 100);
            continue;
        }
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
                if(audio->packetQueue->getQueueSize() > 0)
                {
                    av_usleep(1000 * 100);
                    continue;
                } else{
                    if(!playstatus->seek)
                    {
                        av_usleep(1000 * 100);
                        playstatus->exit = true;
                    }
                    break;
                }
            }
        }
    }
    loge("解码結束");
    if(callJava != NULL)
    {
        callJava->onCallComplete(CHILD_THREAD);
    }
    isExit = true;
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
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    }
    // 解码器分配内存并且赋值给 解码器上下文
    *avCodecContext = avcodec_alloc_context3(avCodec);
    if (!audio->avCodecContext) {
        loge("解码器上下文赋值失败");
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    }
    // 解码器参数传递给解码器上下文
    if (avcodec_parameters_to_context(*avCodecContext, avCodecParameters) < 0) {
        loge("解码器参数传递给解码器上下文失败");
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    }
    // 打开解码器
    if (avcodec_open2(*avCodecContext, avCodec, NULL)) {
        loge("打开解码器失败");
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    };
}

/**
 * 暂停播放
 */
void FFmpegHandle::pausePlay() {
    if(playstatus != NULL)
    {
        playstatus->pause = true;
    }

    if(audio != NULL)
    {
        audio->pausePlay();
    }
}

/**
 * 恢复播放
 */
void FFmpegHandle::resumePlay() {
    if(playstatus != NULL)
    {
        playstatus->pause = false;
    }

    if(audio != NULL)
    {
        audio->resumePlay();
    }
}
/**
 * 停止播放
 */
void FFmpegHandle::stopPlay() {
    loge("释放 playstatus");
    if(playstatus != NULL)
    {
        playstatus->exit = true;
    }
    pthread_join(initAcodecThread, NULL);

    pthread_mutex_lock(&init_mutex);
    int sleepCount = 0;
    while (!isExit)
    {
        if(sleepCount > 1000)
        {
            isExit = true;
        }
        sleepCount++;
        av_usleep(1000 * 10);//暂停10毫秒
    }

    if(audio != NULL)
    {
        loge("释放 audio");
        audio->release();
        delete(audio);
        audio = NULL;
    }

    if(video != NULL)
    {
        loge("释放 video");
        video->release();
        delete(video);
        video = NULL;
    }

    if(avFormatContext != NULL)
    {
        loge("释放 avFormatContext");
        avformat_close_input(&avFormatContext);
        avformat_free_context(avFormatContext);
        avFormatContext = NULL;
    }
    if(callJava != NULL)
    {
        callJava = NULL;
    }
    if(playstatus != NULL)
    {
        playstatus = NULL;
    }
    pthread_mutex_unlock(&init_mutex);
}


void FFmpegHandle::seek(int64_t secds) {

    if(duration <= 0)
    {
        return;
    }
    if(secds >= 0 && secds <= duration)
    {
        playstatus->seek = true;
        pthread_mutex_lock(&seek_mutex);
        int64_t rel = secds * AV_TIME_BASE;
        loge("rel time %d", secds);
        avformat_seek_file(avFormatContext, -1, INT64_MIN, rel, INT64_MAX, 0);
        if(audio != NULL)
        {
            audio->packetQueue->clearAvpacket();
            audio->clock = 0;
            audio->last_time = 0;
            pthread_mutex_lock(&audio->codecMutex);
            avcodec_flush_buffers(audio->avCodecContext);
            pthread_mutex_unlock(&audio->codecMutex);
        }
        if(video != NULL)
        {
            video->packetQueue->clearAvpacket();
            video->clock = 0;
            pthread_mutex_lock(&video->mutex);
            avcodec_flush_buffers(video->avCodecContext);
            pthread_mutex_unlock(&video->mutex);
        }
        pthread_mutex_unlock(&seek_mutex);
        playstatus->seek = false;
    }
}
