//
//  HelloiOSCallback.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/21.
//

#ifndef HelloiOSCallback_hpp
#define HelloiOSCallback_hpp

#include <stdio.h>

#include "player/HelloPlayerCallback.hpp"
#include "HelloiOSCallbackCtx.hpp"


class HelloiOSCallback : public HelloPlayerCallback {
public:
    explicit HelloiOSCallback(void* userdata = nullptr);
    ~HelloiOSCallback() override;

    void onPrepared(const Hello::MediaInfo &info) override;

    /**
     * 播放状态改变 HelloPlayer.cpp::PlayState
     * @param from 从这个状态
     * @param to 变到这个状态
     */
    void onPlayStateChanged(int from, int to) override;

    /**
     * 错误信息回调
     * @param error 错误码 对应 HelloPlayer::ErrorCode
     * @param msg 错误信息
     */
    void onErrorInfo(int error, const char* msg) override;

    /**
     * @param ptsUs 回调当前播放时间，单位：us
     * @param durationUs 总时长，单位：us
     */
    void onCurrentPosition(int64_t ptsUs, int64_t durationUs) override;

    /**
     * 回调当前缓冲区范围
     * @param startUs 缓冲区起始时间，单位：us
     * @param endUs 缓冲区结束时间，单位：us
     * @param durationUs 总时长，单位：us
     */
    void onBufferPosition(int64_t startUs, int64_t endUs, int64_t durationUs) override;

private:
    HelloiOSCallbackCtx *callbacks;
};

#endif /* HelloiOSCallback_hpp */
