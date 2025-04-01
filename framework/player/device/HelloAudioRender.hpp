//
// Created by 廖海龙 on 2025/2/28.
//

#ifndef HELLOPLAYER_HELLOAUDIORENDER_HPP
#define HELLOPLAYER_HELLOAUDIORENDER_HPP

#include "../../log/Logger.hpp"
#include "../entity/AVProperties.hpp"
#include <cstdint>

/**
 * Author: liaohailong
 * Date: 2025/2/28
 * Time: 13:42
 * Description: 音频渲染器接口,由平台侧实现
 **/
class HelloAudioRender {
public:
    typedef void (*Callback)(HelloAudioRender *render,
                             void *audioData, int32_t numFrames, void *userdata);

public:
    explicit HelloAudioRender(const char *tag);

    virtual ~HelloAudioRender();

    /**
     * 配置音频基本信息: 采样率,声道数,位深
     * @param properties 外部配置信息
     * @return 返回音频渲染器调整之后的配置信息，因为外部传入的配置可能不支持，需要调整
     */
    virtual std::shared_ptr<AudioProperties> setConfig(std::shared_ptr<AudioProperties> properties) = 0;

    /**
     * @param framesPerCallback 设置一次回调写入多少个样本, 一个样本 = 声道数 * 位深字节大小
     */
    virtual void setFramesPerCallback(int framesPerCallback) = 0;

    /**
     * @param callback 设置 填充pcm 数据回调接口
     * @param userdata 用户携带参数
     */
    virtual void setCallback(Callback callback, void *userdata) = 0;

    /**
     * 开启音频渲染器,开始触发 Callback 回调
     */
    virtual bool start() = 0;

    /**
     * @return 一个样本占多少字节
     */
    virtual int32_t getBytesPerFrame() = 0;
    
    /**
     * @return 获取延迟帧数
     */
    virtual int64_t getDelayFrameCount();

    /**
     * 停止音频渲染器,停止触发 Callback 回调
     */
    virtual void stop() = 0;


protected:
    Logger logger;
};


#endif //HELLOPLAYER_HELLOAUDIORENDER_HPP
