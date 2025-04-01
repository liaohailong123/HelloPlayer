//
// Created by 廖海龙 on 2025/2/28.
//

#ifndef HELLOPLAYER_HELLOOBOAUDIORENDER_HPP
#define HELLOPLAYER_HELLOOBOAUDIORENDER_HPP

#include "framework/player/device/HelloAudioRender.hpp"

// Obo库
#include <oboe/Oboe.h>
#include <oboe/Utilities.h>

extern "C"
{
#include <libavformat/avformat.h>
}

/**
 * Author: liaohailong
 * Date: 2025/2/28
 * Time: 14:03
 * Description: Android平台,由Obo库实现音频渲染
 **/
class HelloOboAudioRender : public HelloAudioRender, public oboe::AudioStreamCallback {
public:
    explicit HelloOboAudioRender();

    ~HelloOboAudioRender() override;

    std::shared_ptr<AudioProperties> setConfig(std::shared_ptr<AudioProperties> properties) override;

    void setFramesPerCallback(int framesPerCallback) override;

    void setCallback(Callback callback, void *userdata) override;

    bool start() override;

    int32_t getBytesPerFrame() override;

    void stop() override;

private:
    /**
     * 把FFmpeg的声道类型跟Obo库的声道类型转换
     * @param channelCount ffmpeg的声道类型
     * @return 转好的Obo库的声道类型
     */
    oboe::ChannelMask getOboeChannelMask(int channelCount);

    oboe::AudioStreamBuilder *setupCommonStreamParameters(
            oboe::AudioStreamBuilder *builder);

    oboe::AudioStreamBuilder *setupPlaybackStreamParameters(
            oboe::AudioStreamBuilder *builder);

    void warnIfNotLowLatency(std::shared_ptr<oboe::AudioStream> &stream);

    /*
     * oboe::AudioStreamDataCallback interface implementation
     */
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream,
                                          void *audioData, int32_t numFrames) override;

    void closeStreams();

    void closeStream(std::shared_ptr<oboe::AudioStream> &stream);

private:
    /**
     * 使用AAudio接口还是OpenSLES接口
     */
    oboe::AudioApi mAudioApi = oboe::AudioApi::AAudio;

    /**
     * 播放设备id，java层从 AudioManager 中获取 AudioDeviceInfo.id
     */
    int32_t mPlaybackDeviceId = oboe::kUnspecified;

    // 采样率，声道数，位深
    int32_t mSampleRate = oboe::kUnspecified;
    oboe::ChannelMask mChannelMask = oboe::ChannelMask::Stereo;
    oboe::AudioFormat mFormat = oboe::AudioFormat::Float; // for easier processing

    int framesPerCallback;

    Callback callback;
    void* userdata;

    /**
     * 音频播放器流
     */
    std::shared_ptr<oboe::AudioStream> mPlayStream;

};


#endif //HELLOPLAYER_HELLOOBOAUDIORENDER_HPP
