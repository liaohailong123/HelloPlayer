//
// Created by liaohailong on 2025/1/11.
//

#ifndef HELLOPLAYER_PLAYCONFIG_HPP
#define HELLOPLAYER_PLAYCONFIG_HPP

#include <cstdint>


/**
 * create by liaohailong
 * 2025/1/11 10:28
 * desc: 播放配置,这里全写基本类型,不要存指针对象!
 */
struct PlayConfig
{
    /**
     * 数据缓冲区大小，单位：us
     */
    int64_t bufferDurationUs = 10 * 1000 * 1000; // 默认10s

    /**
     * 缓冲区扩充因子
     */
    double bufferLoadMoreFactor = 0.75f;

    /**
     * 是否启用 硬件 来解码视频
     */
    bool useHardware = false;

    /**
     * 自动播放
     */
    bool autoPlay = false;

    /**
     * 循环播放
     */
    bool loopPlay = false;

    /**
     * 音量大小 0.0=静音 1.0=原始音量 大于1.0=扩大音量
     */
    double volume = 1.0;

    /**
     * 播放速率 [0.5,10.0]
     */
    double speed = 1.0;
    
    /**
     * IO连接(网络IO,文件IO)超时,默认15s
     */
    long ioTimeoutUs = 1000 * 1000 * 15;
};


#endif //HELLOPLAYER_PLAYCONFIG_HPP
