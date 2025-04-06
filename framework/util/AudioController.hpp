//
// Created by 廖海龙 on 2025/1/16.
//

#ifndef HELLOPLAYER_AUDIOCONTROLLER_HPP
#define HELLOPLAYER_AUDIOCONTROLLER_HPP

#include <stdio.h>
#include <mutex>
#include <cstdio>
#include <cstdint>
#include <string>
#include <memory>

#include "../log/Logger.hpp"
#include "FFUtil.hpp"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
}

/**
 * Author: liaohailong
 * Date: 2025/1/16
 * Time: 15:38
 * Description: 音频控制器：音量大小 + 音频变速
 **/
class AudioController
{
public:
    AudioController();
    virtual ~AudioController();

    bool setAudioInput(AVRational timeBase, uint32_t sampleRate,
                       AVChannelLayout channelLayout,AVSampleFormat format);
    bool setAudioOutput(uint32_t sampleRate, AVChannelLayout channelLayout,
                        AVSampleFormat format);
    void setVolume(double value);
    void setSpeed(double value);

    bool init();
    bool release();

    bool addFrame(AVFrame* frame);
    AVFrame* getFrame();
private:
    AVFilterContext * createFilter(const char* name, const char *opt);

private:
    typedef struct AudioInfo
    {
        AudioInfo() : name("unknow")
        {

            filterCtx = nullptr;
            rate = 1.0;
            volume = 1.0;
        }
        ~AudioInfo()
        {
            if(filterCtx)
            {
                avfilter_free(filterCtx);
                filterCtx = nullptr;

            }
        }

        AVRational timeBase;
        uint32_t sampleRate;
        AVChannelLayout chanelLayout;
        uint32_t bitsPerSample;
        AVSampleFormat format;
        std::string name;
        double volume;
        double rate;

        AVFilterContext *filterCtx;
    }AudioInfo;

    Logger logger;
    bool initialized;
    std::mutex mutex;
    std::shared_ptr<AudioInfo> audioInputInfo;  // 输入源
    std::shared_ptr<AudioInfo> audioVolumeInfo; // 音量大小
    std::shared_ptr<AudioInfo> audioRateInfo; // 播放速度
    std::shared_ptr<AudioInfo> audioOutputInfo; // 重采样
    std::shared_ptr<AudioInfo> audioSinkInfo; // 输出

    AVFilterGraph *filterGraph;

    double rate;
    double volume;

    /**
     * 第一个送入 AudioController 的frame记录pts，
     * 后面从 AudioController 获取到AVFrame之后，
     * 需要根据该值重新矫正pts
     */
    int64_t firstFramePts;

};


#endif //HELLOPLAYER_AUDIOCONTROLLER_HPP
