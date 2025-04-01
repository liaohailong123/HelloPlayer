//
//  HelloiOSCallback.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/21.
//

#include "HelloiOSCallback.hpp"


HelloiOSCallback::HelloiOSCallback(void* userdata) : HelloPlayerCallback(userdata),
    callbacks((HelloiOSCallbackCtx*)userdata)
{
    logger.i("HelloiOSCallback::HelloiOSCallback(%p)", this);
}
HelloiOSCallback::~HelloiOSCallback()
{
    delete callbacks;
    logger.i("HelloiOSCallback::~HelloiOSCallback(%p)", this);
}

void HelloiOSCallback::onPrepared(const Hello::MediaInfo &info)
{
    if (!callbacks) {
        logger.i("onPrepared callbacks is nullptr");
        return;
    }
    if (!callbacks->onPrepared) {
        logger.i("onPrepared is nullptr");
        return;
    }
    // 主线程回调
    dispatch_async(dispatch_get_main_queue(), ^{
        callbacks->onPrepared(info, callbacks->userdata);
    });
}

/**
 * 播放状态改变 HelloPlayer.cpp::PlayState
 * @param from 从这个状态
 * @param to 变到这个状态
 */
void HelloiOSCallback::onPlayStateChanged(int from, int to)
{
    if (!callbacks) {
        logger.i("onPrepared callbacks is nullptr");
        return;
    }
    if (!callbacks->onPlayStateChanged) {
        logger.i("onPlayStateChanged is nullptr");
        return;
    }
    // 主线程回调
    dispatch_async(dispatch_get_main_queue(), ^{
        callbacks->onPlayStateChanged((PlayState)from, (PlayState)to, callbacks->userdata);
    });
}

/**
 * 错误信息回调
 * @param error 错误码 对应 HelloPlayer::ErrorCode
 * @param msg 错误信息
 */
void HelloiOSCallback::onErrorInfo(int error, const char* msg)
{
    if (!callbacks) {
        logger.i("onErrorInfo callbacks is nullptr");
        return;
    }
    if (!callbacks->onErrorInfo) {
        logger.i("onErrorInfo is nullptr");
        return;
    }
    // 主线程回调
    dispatch_async(dispatch_get_main_queue(), ^{
        NSString *msgStr = [NSString stringWithUTF8String:msg];
        callbacks->onErrorInfo(error,msgStr,callbacks->userdata);
    });
}

/**
 * @param ptsUs 回调当前播放时间，单位：us
 * @param durationUs 总时长，单位：us
 */
void HelloiOSCallback::onCurrentPosition(int64_t ptsUs, int64_t durationUs)
{
    if (!callbacks) {
        logger.i("onCurrentPosition callbacks is nullptr");
        return;
    }
    if (!callbacks->onCurrentPosition) {
        logger.i("onCurrentPosition is nullptr");
        return;
    }
    // 主线程回调
    dispatch_async(dispatch_get_main_queue(), ^{
        callbacks->onCurrentPosition(ptsUs, durationUs, callbacks->userdata);
    });
}

/**
 * 回调当前缓冲区范围
 * @param startUs 缓冲区起始时间，单位：us
 * @param endUs 缓冲区结束时间，单位：us
 * @param durationUs 总时长，单位：us
 */
void HelloiOSCallback::onBufferPosition(int64_t startUs, int64_t endUs, int64_t durationUs)
{
    if (!callbacks) {
        logger.i("onBufferPosition callbacks is nullptr");
        return;
    }
    if (!callbacks->onBufferPosition) {
        logger.i("onBufferPosition is nullptr");
        return;
    }
    // 主线程回调
    dispatch_async(dispatch_get_main_queue(), ^{
        callbacks->onBufferPosition(startUs, endUs, durationUs, callbacks->userdata);
    });
}
