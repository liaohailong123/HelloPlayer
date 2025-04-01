package com.example.module_rtsp;

import androidx.annotation.NonNull;

/**
 * create by liaohailong
 * 2025/1/11 12:22
 * desc:
 */
public class HelloPlayerMediaInfo {
    @NonNull
    public final String url;

    public final long startOffsetUs; // 播放起始偏移量
    public final long audioDurationUs; // 音频播放时长，单位：us
    public final long videoDurationUs; // 视频播放时长，单位：us

    public final int sampleRate; // 音频采样率
    public final int channelCount; // 音频声道数
    /**
     * samplefmt.h
     * enum AVSampleFormat {
     * AV_SAMPLE_FMT_NONE = -1,
     * AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
     * AV_SAMPLE_FMT_S16,         ///< signed 16 bits
     * AV_SAMPLE_FMT_S32,         ///< signed 32 bits
     * AV_SAMPLE_FMT_FLT,         ///< float
     * AV_SAMPLE_FMT_DBL,         ///< double
     * <p>
     * AV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
     * AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
     * AV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
     * AV_SAMPLE_FMT_FLTP,        ///< float, planar
     * AV_SAMPLE_FMT_DBLP,        ///< double, planar
     * AV_SAMPLE_FMT_S64,         ///< signed 64 bits
     * AV_SAMPLE_FMT_S64P,        ///< signed 64 bits, planar
     * <p>
     * AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
     * };
     */
    public final int sampleFmt; // 音频样本位深 AVSampleFormat

    public final int width; // 视频宽
    public final int height; // 视频高
    public final int frameRate; // 视频播放帧率

    /**
     * 主时钟类型：
     * AUDIO=0 VIDEO=1 参考：IAVMediaType.hpp
     */
    public final int masterClockType;

    /**
     * 是否为直播流
     */
    public final boolean isLiveStreaming;


    public HelloPlayerMediaInfo(@NonNull String url,
                                long startOffsetUs,
                                long audioDurationUs,
                                long videoDurationUs,
                                int sampleRate, int channelCount, int sampleFmt,
                                int width, int height, int frameRate,int masterClockType,
                                boolean isLiveStreaming) {
        this.url = url;
        this.startOffsetUs = startOffsetUs;
        this.audioDurationUs = audioDurationUs;
        this.videoDurationUs = videoDurationUs;
        this.sampleRate = sampleRate;
        this.channelCount = channelCount;
        this.sampleFmt = sampleFmt;
        this.width = width;
        this.height = height;
        this.frameRate = frameRate;
        this.masterClockType = masterClockType;
        this.isLiveStreaming = isLiveStreaming;
    }

    public long getDurationUs() {
        return masterClockType == 0 ? audioDurationUs : videoDurationUs;
    }

    @NonNull
    @Override
    public String toString() {
        return "HelloPlayerMediaInfo{" +
                "url='" + url + '\'' +
                ", startOffsetUs=" + startOffsetUs +
                ", audioDurationUs=" + audioDurationUs +
                ", videoDurationUs=" + videoDurationUs +
                ", sampleRate=" + sampleRate +
                ", channelCount=" + channelCount +
                ", sampleFmt=" + sampleFmt +
                ", width=" + width +
                ", height=" + height +
                ", frameRate=" + frameRate +
                ", masterClockType=" + masterClockType +
                '}';
    }
}
