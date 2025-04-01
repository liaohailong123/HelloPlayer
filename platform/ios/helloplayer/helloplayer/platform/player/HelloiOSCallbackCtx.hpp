//
//  HelloiOSCallbackCtx.h
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/21.
//

#ifndef HelloiOSCallbackCtx_hpp
#define HelloiOSCallbackCtx_hpp

#import "HelloPlayerState.h"

#include "player/HelloPlayerCallback.hpp"

class HelloiOSCallbackCtx
{
public:
    /**
     * 资源已经准备好了,可以开始播放
     * @param info 多媒体文件基本信息
     */
    typedef void (*OnPrepared)(const Hello::MediaInfo &info, void* userdata);
    /**
     * 播放状态改变 HelloPlayer.cpp::PlayState
     * @param from 从这个状态
     * @param to 变到这个状态
     */
    typedef void (*OnPlayStateChanged)(PlayState from, PlayState to, void* userdata);
    /**
     * 错误信息回调
     * @param error 错误码 对应 HelloPlayer::ErrorCode
     * @param msg 错误信息
     */
    typedef void (*OnErrorInfo)(int error, NSString *msg, void* userdata);
    /**
     * @param ptsUs 回调当前播放时间，单位：us
     * @param durationUs 总时长，单位：us
     */
    typedef void (*OnCurrentPosition)(int64_t ptsUs, int64_t durationUs, void* userdata);
    /**
     * 回调当前缓冲区范围
     * @param startUs 缓冲区起始时间，单位：us
     * @param endUs 缓冲区结束时间，单位：us
     * @param durationUs 总时长，单位：us
     */
    typedef void (*OnBufferPosition)(int64_t startUs, int64_t endUs, int64_t durationUs, void* userdata);
public:
    HelloiOSCallbackCtx();
    ~HelloiOSCallbackCtx();
    
public:
    void* userdata;
    OnPrepared onPrepared;
    OnPlayStateChanged onPlayStateChanged;
    OnErrorInfo onErrorInfo;
    OnCurrentPosition onCurrentPosition;
    OnBufferPosition onBufferPosition;
};

#endif /* HelloiOSCallbackCtx_hpp */
