//
// Created by liaohailong on 2024/12/22.
//

#include "HelloAVSync.hpp"

HelloAVSync::HelloAVSync(const char *tag) : logger(tag), type(IAVMediaType::AUDIO),
                                            audioClock("AudioClock"), videoClock("VideoClock")
{
    logger.i("HelloAVSync::HelloAVSync(%p)", this);
}

HelloAVSync::~HelloAVSync()
{
    logger.i("HelloAVSync::HelloAVSync(%p)", this);
}

/**
 * @param mediaType 设置当前 时间轴 的媒体类型，一般以音频为主，除非单轨道的情况
 */
void HelloAVSync::setMastClockType(IAVMediaType mediaType)
{
    type = mediaType;
}

/**
 * @return 获取 当前 时间轴 的媒体类型
 */
const IAVMediaType &HelloAVSync::getMastClockType()
{
    return type;
}

/**
 * @param mediaType 判断该类型是否为 主时钟
 * @return true表示为主时钟
 */
bool HelloAVSync::isMasterClock(const IAVMediaType &mediaType)
{
    return getMastClockType() == mediaType;
}

HelloClock &HelloAVSync::getClockByType(const IAVMediaType &mediaType)
{
    // 暂时只处理 这两种时钟
    if (mediaType == AUDIO)
    {
        return audioClock;
    } else
    {
        return videoClock;
    }
}

HelloClock &HelloAVSync::getMasterClock()
{
    return getClockByType(getMastClockType());
}

int64_t HelloAVSync::getMasterClockSerial()
{
    return getMasterClock().serial;
}

void HelloAVSync::setPaused(bool paused)
{
    audioClock.setPaused(paused);
    videoClock.setPaused(paused);
}

void HelloAVSync::reset(int64_t serial)
{
    audioClock.reset(serial);
    videoClock.reset(serial);
}
