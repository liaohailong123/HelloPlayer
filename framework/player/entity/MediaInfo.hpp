//
// Created by liaohailong on 2025/1/11.
//

#ifndef HELLOPLAYER_MEDIAINFO_HPP
#define HELLOPLAYER_MEDIAINFO_HPP

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>  // 用于格式化输出

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

namespace Hello
{
    class MediaInfo
    {
    public:
        std::string url;


        int64_t startOffsetUs;
        int64_t audioDurationUs;
        int64_t videoDurationUs;


        int sampleRate; // 采样率
        int channelCount; // 声道数
        int sampleFmt; // 位深 AVSampleFormat


        int width; // 视频宽
        int height; // 视频高
        AVRational frameRate; // 视频播放帧率

        /**
         * IAVMediaType.hpp
         * 主时钟类型 audio=0 video=1
         */
        int masterClockType;

        /**
         * 是否为直播流
         */
        bool isLiveStreaming;

    public:
        std::string toString()
        {
            std::ostringstream oss;
            oss << "MediaInfo {"
                << "\n  url: " << url
                << "\n  startOffsetUs: " << startOffsetUs
                << "\n  audioDurationUs: " << audioDurationUs
                << "\n  videoDurationUs: " << videoDurationUs
                << "\n  sampleRate: " << sampleRate
                << "\n  channelCount: " << channelCount
                << "\n  sampleFmt: " << sampleFmt
                << "\n  width: " << width
                << "\n  height: " << height
                << "\n  frameRate: " << frameRate.num << "/" << frameRate.den
                << "\n  masterClockType: " << masterClockType
                << "\n}";
            return oss.str();
        }
    };
}

#endif //HELLOPLAYER_MEDIAINFO_HPP
