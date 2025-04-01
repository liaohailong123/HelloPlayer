//
// Created by liaohailong on 2025/1/12.
//

#ifndef HELLOPLAYER_HELLOPLAYERCALLBACK_HPP
#define HELLOPLAYER_HELLOPLAYERCALLBACK_HPP

#include "../log/Logger.hpp"
#include "entity/MediaInfo.hpp"


/**
 * create by liaohailong
 * 2025/1/12 10:25
 * desc: HelloPlayer播放器回调上层
 */
class HelloPlayerCallback {
public:
    static std::shared_ptr<HelloPlayerCallback> createHelloPlayerCallback(void* userdata);
public:
    explicit HelloPlayerCallback(void* userdata = nullptr);

    virtual ~HelloPlayerCallback();

    virtual void onPrepared(const Hello::MediaInfo &info) = 0;

    /**
     * 播放状态改变 HelloPlayer.cpp::PlayState
     * @param from 从这个状态
     * @param to 变到这个状态
     */
    virtual void onPlayStateChanged(int from, int to) = 0;

    /**
     * 错误信息回调
     * @param error 错误码 对应 HelloPlayer::ErrorCode
     * @param msg 错误信息
     */
    virtual void onErrorInfo(int error, const char* msg) = 0;

    /**
     * @param ptsUs 回调当前播放时间，单位：us
     * @param durationUs 总时长，单位：us
     */
    virtual void onCurrentPosition(int64_t ptsUs, int64_t durationUs) = 0;

    /**
     * 回调当前缓冲区范围
     * @param startUs 缓冲区起始时间，单位：us
     * @param endUs 缓冲区结束时间，单位：us
     * @param durationUs 总时长，单位：us
     */
    virtual void onBufferPosition(int64_t startUs, int64_t endUs, int64_t durationUs) = 0;

protected:
    Logger logger;
};


#endif //HELLOPLAYER_HELLOPLAYERCALLBACK_HPP
