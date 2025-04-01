//
// Created on 2025/3/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "HelloOhosCallback.hpp"

HelloOhosCallback::HelloOhosCallback(void *userdata)
    : HelloPlayerCallback(userdata), callbacks((HelloOhosCallbackCtx *)userdata)
{
    logger.i("HelloOhosCallback::HelloOhosCallback(%p)", this);   
}
HelloOhosCallback::~HelloOhosCallback() 
{
    if (callbacks) {
        delete callbacks->onPreparedCallback;
        delete callbacks->onBufferPositionCallback;
        delete callbacks->onCurrentPositionCallback;
        delete callbacks->onErrorInfoCallback;
        delete callbacks->onPlayStateChangedCallback;
        delete callbacks;
        callbacks = nullptr;
    }
    logger.i("HelloOhosCallback::~HelloOhosCallback(%p)", this);   
}

void HelloOhosCallback::onPrepared(const Hello::MediaInfo &info) 
{
    if (!callbacks) {
        logger.i("onPrepared callbacks is nullptr");
        return;
    }
    if (!callbacks->onPreparedCallback) {
        logger.i("onPreparedCallback is nullptr");
        return;
    }
    NapiCallbackData *data = new NapiCallbackData();
    data->str0 = info.url.c_str();
    data->long0 = info.startOffsetUs;
    data->long1 = info.audioDurationUs;
    data->long2 = info.videoDurationUs;
    data->long3 = info.sampleRate;
    data->long4 = info.channelCount;
    data->long5 = info.sampleFmt;
    data->long6 = info.width;
    data->long7 = info.height;
    data->long8 = info.frameRate.num;
    data->long9 = info.masterClockType;
    data->bool0 = info.isLiveStreaming;

    callbacks->onPreparedCallback->call(data);
}

/**
* 播放状态改变 HelloPlayer.cpp::PlayState
* @param from 从这个状态
* @param to 变到这个状态
*/
void HelloOhosCallback::onPlayStateChanged(int from, int to)
{
    if (!callbacks) {
        logger.i("onPlayStateChanged callbacks is nullptr");
        return;
    }
    if (!callbacks->onPlayStateChangedCallback) {
        logger.i("onPlayStateChangedCallback is nullptr");
        return;
    }
    NapiCallbackData *data = new NapiCallbackData();

    data->long0 = from;
    data->long1 = to;

    callbacks->onPlayStateChangedCallback->call(data);
}

/**
* 错误信息回调
* @param error 错误码 对应 HelloPlayer::ErrorCode
* @param msg 错误信息
*/
void HelloOhosCallback::onErrorInfo(int error, const char* msg) 
{
    if (!callbacks) {
        logger.i("onErrorInfo callbacks is nullptr");
        return;
    }
    if (!callbacks->onErrorInfoCallback) {
        logger.i("onErrorInfoCallback is nullptr");
        return;
    }
    NapiCallbackData *data = new NapiCallbackData();

    data->long0 = error;
    data->str0 = msg;

    callbacks->onErrorInfoCallback->call(data);
}

/**
* @param ptsUs 回调当前播放时间，单位：us
* @param durationUs 总时长，单位：us
*/
void HelloOhosCallback::onCurrentPosition(int64_t ptsUs, int64_t durationUs) 
{
    if (!callbacks) {
        logger.i("onCurrentPosition callbacks is nullptr");
        return;
    }
    if (!callbacks->onCurrentPositionCallback) {
        logger.i("onCurrentPositionCallback is nullptr");
        return;
    }
    NapiCallbackData *data = new NapiCallbackData();

    data->long0 = ptsUs;
    data->long1 = durationUs;

    callbacks->onCurrentPositionCallback->call(data);
}

/**
* 回调当前缓冲区范围
* @param startUs 缓冲区起始时间，单位：us
* @param endUs 缓冲区结束时间，单位：us
* @param durationUs 总时长，单位：us
*/
void HelloOhosCallback::onBufferPosition(int64_t startUs, int64_t endUs, int64_t durationUs) 
{
    if (!callbacks) {
        logger.i("onBufferPosition callbacks is nullptr");
        return;
    }
    if (!callbacks->onBufferPositionCallback) {
        logger.i("onBufferPositionCallback is nullptr");
        return;
    }
    NapiCallbackData *data = new NapiCallbackData();

    data->long0 = startUs;
    data->long1 = endUs;
    data->long2 = durationUs;

    callbacks->onBufferPositionCallback->call(data);
}
