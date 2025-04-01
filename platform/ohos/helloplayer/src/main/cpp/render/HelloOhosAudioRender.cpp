//
// Created on 2025/3/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "HelloOhosAudioRender.hpp"
#include <cstdint>

HelloOhosAudioRender::HelloOhosAudioRender() : HelloAudioRender("HelloOhosAudioRender"),
                                                mSampleRate(0), mChannelCount(0), mFormat(AUDIOSTREAM_SAMPLE_S16LE),
                                                bytePerSample(0), framesPerCallback(1024),
                                                builder(nullptr), audioRenderer(nullptr), audioCallbacks()
{
    logger.i("HelloOhosAudioRender::HelloOhosAudioRender(%p)", this);
}

HelloOhosAudioRender::~HelloOhosAudioRender()
{
    logger.i("HelloOhosAudioRender::~HelloOhosAudioRender(%p)", this);
}

std::shared_ptr<AudioProperties> HelloOhosAudioRender::setConfig(std::shared_ptr<AudioProperties> p)
{
    std::shared_ptr<AudioProperties> support = std::make_shared<AudioProperties>();
    support->sampleRate = p->sampleRate;
    support->channelCount = p->channelCount;
    support->sampleFmt = p->sampleFmt;
    support->frame_size = p->frame_size;
    
    mSampleRate = p->sampleRate;
    mChannelCount = p->channelCount;
    
    if (p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_S16 ||
        p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_S16P)
    {
        mFormat = AUDIOSTREAM_SAMPLE_S16LE;
        bytePerSample = 2;
        logger.i("sampleFmt: AUDIOSTREAM_SAMPLE_S16LE");
    } else if (p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_S32 ||
               p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_S32P)
    {
        mFormat = AUDIOSTREAM_SAMPLE_S32LE;
        bytePerSample = 4;
        logger.i("sampleFmt: AUDIOSTREAM_SAMPLE_S32LE");
    } else 
    {
        mFormat = AUDIOSTREAM_SAMPLE_S16LE;
        bytePerSample = 2;
        support->sampleFmt = AVSampleFormat::AV_SAMPLE_FMT_S16;
        logger.i("sampleFmt(default): AUDIOSTREAM_SAMPLE_S16LE");
    }
    
    return support;
}

void HelloOhosAudioRender::setFramesPerCallback(int _framesPerCallback)
{
    this->framesPerCallback = _framesPerCallback;
    logger.i("setFramesPerCallback: %d", framesPerCallback);
}

void HelloOhosAudioRender::setCallback(Callback _callback, void *_userdata)
{
    this->callback = _callback;
    this->userdata = _userdata;
}

bool HelloOhosAudioRender::start()
{
    OH_AudioStreamBuilder_Create(&builder, AUDIOSTREAM_TYPE_RENDERER);

    // 设置音频采样率
    OH_AudioStreamBuilder_SetSamplingRate(builder, mSampleRate);
    // 设置音频声道
    OH_AudioStreamBuilder_SetChannelCount(builder, mChannelCount);
    // 设置音频采样格式
    OH_AudioStreamBuilder_SetSampleFormat(builder, mFormat);
    // 设置音频流的编码类型
    OH_AudioStreamBuilder_SetEncodingType(builder, AUDIOSTREAM_ENCODING_TYPE_RAW);
    // 设置输出音频流的工作场景
    OH_AudioStreamBuilder_SetRendererInfo(builder, AUDIOSTREAM_USAGE_MUSIC);
    
    OH_AudioStream_LatencyMode latencyMode = AUDIOSTREAM_LATENCY_MODE_FAST;
    OH_AudioStreamBuilder_SetLatencyMode(builder, latencyMode);
    
    // 多种回调
    audioCallbacks.OH_AudioRenderer_OnWriteData = HelloOhosAudioRender::OH_AudioRenderer_OnWriteData; // 写PCM数据
    audioCallbacks.OH_AudioRenderer_OnError = HelloOhosAudioRender::OH_AudioRenderer_OnError;
    audioCallbacks.OH_AudioRenderer_OnInterruptEvent = HelloOhosAudioRender::OH_AudioRenderer_OnInterruptEvent;
    audioCallbacks.OH_AudioRenderer_OnStreamEvent = HelloOhosAudioRender::OH_AudioRenderer_OnStreamEvent;
    
    OH_AudioStreamBuilder_SetRendererCallback(builder, audioCallbacks, this);
    OH_AudioStreamBuilder_SetFrameSizeInCallback(builder, this->framesPerCallback);
    int ret = OH_AudioStreamBuilder_GenerateRenderer(builder, &audioRenderer);
    logger.i("OH_AudioStreamBuilder_GenerateRenderer ret:%d, sample_rate: %d, channels: %d, format: %d", mSampleRate,
             mChannelCount, mFormat);
    // 开启音频渲染器
    if (audioRenderer && AUDIOSTREAM_SUCCESS == ret) {
        if (audioRenderer) {
            ret = OH_AudioRenderer_Start(audioRenderer);
            
            // 设置当前音频流音量值
            OH_AudioRenderer_SetVolume(audioRenderer, 1.0f);
        }
    }
    return AUDIOSTREAM_SUCCESS == ret;
}

int32_t HelloOhosAudioRender::getBytesPerFrame()
{
    return mChannelCount * bytePerSample;
}

int64_t HelloOhosAudioRender::getDelayFrameCount()
{
    // 测一下音频缓冲区大小 真机华为mate60 11个空包(11*1024帧)的延迟
    return 11 * framesPerCallback;
}

void HelloOhosAudioRender::stop()
{
    if(audioRenderer)
    {
        OH_AudioStream_Result ret = OH_AudioRenderer_Stop(audioRenderer);
        ret = OH_AudioRenderer_Release(audioRenderer);
        audioRenderer = nullptr;
    }

    if (builder) {
        OH_AudioStreamBuilder_Destroy(builder);
        builder = nullptr;
    }
}

int32_t HelloOhosAudioRender::OH_AudioRenderer_OnWriteData(OH_AudioRenderer *renderer, void *userData, void *buffer, int32_t length)
{
    auto native = reinterpret_cast<HelloOhosAudioRender*>(userData);
    
    // 一共多少个样本
    int32_t frameSize = 0;
    OH_AudioRenderer_GetFrameSizeInCallback(renderer, &frameSize);

    if (native->callback)
    {
        native->callback(native, buffer, frameSize, native->userdata);
    } else {
        // 默认静音
        memset(buffer, 0x00, length);
    }
    
    // 测一下音频缓冲区大小 真机华为mate60 11个空包(11*1024帧)的延迟
//    int64_t framePosition = 0;
//    int64_t timestamp = 0;
//    OH_AudioStream_Result ret = OH_AudioRenderer_GetTimestamp(renderer, CLOCK_MONOTONIC, &framePosition, &timestamp);
//    if (ret == AUDIOSTREAM_SUCCESS) {
//        native->logger.i("OH_AudioRenderer_GetTimestamp framePosition[%ld] timestamp[%ld] frameSize[%d]", framePosition,
//                         timestamp, frameSize);
//    }
    return 0;
}
int32_t HelloOhosAudioRender::OH_AudioRenderer_OnError(OH_AudioRenderer *renderer, void *userData, OH_AudioStream_Result error)
{
    auto native = reinterpret_cast<HelloOhosAudioRender*>(userData);
    native->logger.i("OH_AudioRenderer_OnError error: %d", error);
    return 0;
}
int32_t HelloOhosAudioRender::OH_AudioRenderer_OnInterruptEvent(OH_AudioRenderer *renderer, void *userData,
                                                 OH_AudioInterrupt_ForceType type, OH_AudioInterrupt_Hint hint)
{
    auto native = reinterpret_cast<HelloOhosAudioRender*>(userData);
    native->logger.i("OH_AudioRenderer_OnInterruptEvent type: %d, hint: %d", type, hint);
    return 0;
}
int32_t HelloOhosAudioRender::OH_AudioRenderer_OnStreamEvent(OH_AudioRenderer *renderer, void *userData, OH_AudioStream_Event event)
{
    auto native = reinterpret_cast<HelloOhosAudioRender*>(userData);
    native->logger.i("OH_AudioRenderer_OnStreamEvent event: %d", event);
    return 0;
}
