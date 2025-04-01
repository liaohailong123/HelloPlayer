//
// Created on 2025/3/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OHOS_HELLOOHOSAUDIORENDER_H
#define OHOS_HELLOOHOSAUDIORENDER_H

#include "framework/player/device/HelloAudioRender.hpp"

// 鸿蒙系统提供的音频播放库
#include <cstdint>
#include <ohaudio/native_audiorenderer.h>
#include <ohaudio/native_audiostreambuilder.h>

extern "C"
{
#include <libavformat/avformat.h>
}

/**
 * 鸿蒙平台,由 native_audiorender 实现音频渲染
 * 参考官方示例:
 * https://developer.huawei.com/consumer/cn/doc/harmonyos-guides-V14/using-ohaudio-for-playback-V14
 */
class HelloOhosAudioRender : public HelloAudioRender{
public:
    explicit HelloOhosAudioRender();

    ~HelloOhosAudioRender() override;

    std::shared_ptr<AudioProperties> setConfig(std::shared_ptr<AudioProperties> properties) override;

    void setFramesPerCallback(int framesPerCallback) override;

    void setCallback(Callback callback, void *userdata) override;

    bool start() override;

    int32_t getBytesPerFrame() override;
    
    int64_t getDelayFrameCount() override;

    void stop() override;

private:
    // 已下都是鸿蒙 native audio 库内部回调
    static int32_t OH_AudioRenderer_OnWriteData(OH_AudioRenderer *renderer, void *userData, void *buffer, int32_t length);
    static int32_t OH_AudioRenderer_OnError(OH_AudioRenderer *renderer, void *userData, OH_AudioStream_Result error);
    static int32_t OH_AudioRenderer_OnInterruptEvent(OH_AudioRenderer *renderer, void *userData,
                                                 OH_AudioInterrupt_ForceType type, OH_AudioInterrupt_Hint hint);
    static int32_t OH_AudioRenderer_OnStreamEvent(OH_AudioRenderer *renderer, void *userData, OH_AudioStream_Event event);
    
private:
    
    // 采样率，声道数，位深
    int32_t mSampleRate = 0;
    int32_t mChannelCount;
    OH_AudioStream_SampleFormat mFormat;
    int32_t bytePerSample;
    /**
     * 指定每次回调，写入多少个音频帧（一帧 = 声道数 * 位深占字节大小 ）
     */
    int framesPerCallback;

    Callback callback;
    void* userdata;

    
    OH_AudioStreamBuilder *builder = nullptr;
    OH_AudioRenderer *audioRenderer = nullptr;
    OH_AudioRenderer_Callbacks audioCallbacks;

};

#endif //OHOS_HELLOOHOSAUDIORENDER_H
