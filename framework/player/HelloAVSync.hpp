//
// Created by liaohailong on 2024/12/22.
//

#ifndef HELLOPLAYER_HELLOAVSYNC_HPP
#define HELLOPLAYER_HELLOAVSYNC_HPP

#include <cstdint>

#include "../log/Logger.hpp"
#include "entity/IAVMediaType.hpp"
#include "clock/HelloClock.hpp"


/**
 * create by liaohailong
 * 2024/12/22 12:51
 * desc: 播放器时间轴
 * 策略：
 * 1，存在多轨道时，以音频为主
 * 2，单轨道时，以单轨道为主
 */
class HelloAVSync
{
public:
    explicit HelloAVSync(const char *tag = "HelloAVSync");

    ~HelloAVSync();

    /**
     * @param mediaType 设置当前 时间轴 的媒体类型，一般以音频为主，除非单轨道的情况
     */
    void setMastClockType(IAVMediaType mediaType);

    /**
     * @return 获取 当前 时间轴 的媒体类型
     */
    IAVMediaType getMastClockType();

    /**
     * @param mediaType 判断该类型是否为 主时钟
     * @return true表示为主时钟
     */
    bool isMasterClock(IAVMediaType mediaType);

    /**
     * @param mediaType 获取对应的时钟
     * @return
     */
    HelloClock &getClockByType(IAVMediaType mediaType);

    HelloClock &getMasterClock();

    int64_t getMasterClockSerial();

    /**
     * @param paused true表示时钟暂停
     */
    void setPaused(bool paused);

    /**
     * @param serial 使用新序列号，初始化时钟
     */
    void reset(int64_t serial);

private:
    /**
     * 日志打印
     */
    Logger logger;

    /**
     * 多媒体类型，一般以音频为时间轴，单轨道除外
     */
    IAVMediaType type;

    /**
     * 音频时钟
     */
    HelloClock audioClock;
    /**
     * 视频时钟
     */
    HelloClock videoClock;

};


#endif //HELLOPLAYER_HELLOAVSYNC_HPP
