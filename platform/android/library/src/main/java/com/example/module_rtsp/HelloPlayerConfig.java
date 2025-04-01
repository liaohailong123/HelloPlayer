package com.example.module_rtsp;

/**
 * create by liaohailong
 * 2025/1/11 12:17
 * desc:
 */
public class HelloPlayerConfig {

    /**
     * 数据缓冲区大小，单位：us
     */
    public long bufferDurationUs = 10 * 1000 * 1000; // 默认10s

    /**
     * 缓冲区扩充因子
     */
    public double bufferLoadMoreFactor = 0.75f;

    /**
     * 是否启用 MediaCodec 来解码视频
     */
    public boolean useHardware = false;

    /**
     * 自动播放
     */
    public boolean autoPlay = false;

    /**
     * 循环播放
     */
    public boolean loopPlay = false;

    /**
     * 音量大小 0.0=静音 1.0=原始音量 大于1.0=扩大音量
     */
    public double volume = 1.0;

    /**
     * 播放速率 [0.5,10.0]
     */
    public double speed = 1.0;

    /**
     * IO连接(网络IO,文件IO)超时,默认15s
     */
    public long ioTimeoutUs = 1000 * 1000 * 15;
}
