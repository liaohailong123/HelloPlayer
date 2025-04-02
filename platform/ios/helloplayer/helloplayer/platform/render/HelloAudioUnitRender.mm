//
//  HelloAudioUnitRender.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/21.
//

#include "HelloAudioUnitRender.hpp"

#define kOutputBus 0
#define kInputBus 1

#define CHECK_ERROR(error, desc) \
if (error != noErr) { \
    char formatID[5]; \
    *(UInt32 *)formatID = CFSwapInt32HostToBig(error); \
    formatID[4] = '\0'; \
    fprintf(stderr, "'%s'! %d '%-4.4s'\n", desc, error, formatID); \
}

HelloAudioUnitRender::HelloAudioUnitRender() : HelloAudioRender("HelloAudioUnitRender"),sampleRate(44100),channelsPerFrame(2),bitsPerChannel(16),
                                                bytePerSample(2),bytesPerFrame(4),formatFlags(kLinearPCMFormatFlagIsSignedInteger),
                                                framesPerCallback(1024), audioUnit(nullptr), callback(nullptr), userdata(nullptr)
{
    logger.i("HelloAudioUnitRender::HelloAudioUnitRender(%p)", this);
}
HelloAudioUnitRender::~HelloAudioUnitRender()
{
    logger.i("HelloAudioUnitRender::~HelloAudioUnitRender(%p)", this);
}

std::shared_ptr<AudioProperties> HelloAudioUnitRender::setConfig(std::shared_ptr<AudioProperties> p)
{
    std::shared_ptr<AudioProperties> support = std::make_shared<AudioProperties>();
    support->sampleRate = p->sampleRate;
    support->channelCount = p->channelCount;
    support->sampleFmt = p->sampleFmt;
    support->frame_size = p->frame_size;
    
    formatFlags = kLinearPCMFormatFlagIsSignedInteger|kAudioFormatFlagIsPacked;
    if (p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_S16 ||
        p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_S16P)
    {
        bitsPerChannel = 16;
        support->sampleFmt = AVSampleFormat::AV_SAMPLE_FMT_S16;
        logger.i("sampleFmt: AUDIOSTREAM_SAMPLE_S16LE");
    } else if (p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_S32 ||
               p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_S32P)
    {
        bitsPerChannel = 32;
        support->sampleFmt = AVSampleFormat::AV_SAMPLE_FMT_S32;
        logger.i("sampleFmt: AUDIOSTREAM_SAMPLE_S32LE");
    }
    else if (p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_FLT ||
              p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_FLTP)
    {
        bitsPerChannel = 32;
        formatFlags = kLinearPCMFormatFlagIsFloat|kAudioFormatFlagIsPacked;
        support->sampleFmt = AVSampleFormat::AV_SAMPLE_FMT_FLT;
        logger.i("sampleFmt: AudioFormat::Float");
    } else
    {
        bitsPerChannel = 16;
        support->sampleFmt = AVSampleFormat::AV_SAMPLE_FMT_S16;
        logger.i("sampleFmt(default): AUDIOSTREAM_SAMPLE_S16LE");
    }
    
    // 测试发现: iPad mini(第5代) + iPhone 16 Pro 模拟器不支持 6声道
    // 暂时先全部转换成 双声道
    if (p->channelCount > 2) {
        support->channelCount = 2;
    }
    
    sampleRate = support->sampleRate;
    channelsPerFrame = support->channelCount;
    bytePerSample = bitsPerChannel/8;
    bytesPerFrame = bytePerSample * channelsPerFrame;
    
    return support;
}

void HelloAudioUnitRender::setAudioSession()
{
    logger.i("setAudioSession start");
    
    NSError *error = nil;
  
    // set audio session
    AVAudioSession *audioSession = [AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryPlayback error:&error];
    [audioSession setPreferredOutputNumberOfChannels:channelsPerFrame error:&error];
    [audioSession setActive:YES error:&error];
    
    if(error)
    {
        const char *msg = [error.localizedDescription UTF8String];
        logger.i("setAudioSession error: %s", msg);
    }
    NSInteger channels = audioSession.outputNumberOfChannels;
    logger.i("setAudioSession start channelsPerFrame[%d] channels[%d]", channelsPerFrame, channels);
}

void HelloAudioUnitRender::setIOUnit()
{
    // 释放之前的 AudioUnit
    if (audioUnit) {
        AudioOutputUnitStop(audioUnit);
        AudioUnitUninitialize(audioUnit);
        AudioComponentInstanceDispose(audioUnit);
    }
    
    AudioComponentDescription audioDesc;
    audioDesc.componentType = kAudioUnitType_Output;
    audioDesc.componentSubType = kAudioUnitSubType_RemoteIO;
    audioDesc.componentManufacturer = kAudioUnitManufacturer_Apple;
    audioDesc.componentFlags = 0;
    audioDesc.componentFlagsMask = 0;
    
    AudioComponent inputComponent = AudioComponentFindNext(NULL, &audioDesc);
    OSStatus status = AudioComponentInstanceNew(inputComponent, &audioUnit);
    CHECK_ERROR(status, "创建 AudioUnit 失败");
    
    UInt32 enable_out = 1;
    CHECK_ERROR(AudioUnitSetProperty(audioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, kOutputBus, &enable_out, sizeof(enable_out)), "设置 enableOut 失败");
    
    // format
    AudioStreamBasicDescription outputFormat;
    memset(&outputFormat, 0, sizeof(outputFormat));
    outputFormat.mSampleRate       = sampleRate; // 采样率
    outputFormat.mFormatID         = kAudioFormatLinearPCM; // PCM格式
    outputFormat.mFormatFlags      = formatFlags; // 数据类型,整型 or 浮点
    outputFormat.mFramesPerPacket  = 1; // 1个packet里面只有1个frame
    outputFormat.mChannelsPerFrame = channelsPerFrame; // 声道数
    outputFormat.mBytesPerFrame    = bytesPerFrame;
    outputFormat.mBytesPerPacket   = bytesPerFrame;
    outputFormat.mBitsPerChannel   = bitsPerChannel; // 位深

    CHECK_ERROR(AudioUnitSetProperty(audioUnit,
                                     kAudioUnitProperty_StreamFormat,
                                     kAudioUnitScope_Input,
                                     kOutputBus,
                                     &outputFormat,
                                     sizeof(outputFormat)), "设置格式失败");
    
    AURenderCallbackStruct playCallback;
    playCallback.inputProc = onAudioPlayCallbackFunc;
    playCallback.inputProcRefCon = this;
    CHECK_ERROR(AudioUnitSetProperty(audioUnit,
                                     kAudioUnitProperty_SetRenderCallback,
                                     kAudioUnitScope_Input,
                                     kOutputBus,
                                     &playCallback,
                                     sizeof(playCallback)), "设置回调失败");
    CHECK_ERROR(AudioUnitInitialize(audioUnit), "初始化失败");
}

void HelloAudioUnitRender::setFramesPerCallback(int _framesPerCallback)
{
    this->framesPerCallback = _framesPerCallback;
    logger.i("setFramesPerCallback: %d", _framesPerCallback);
}

void HelloAudioUnitRender::setCallback(Callback _callback, void *_userdata)
{
    this->callback = _callback;
    this->userdata = _userdata;
}

bool HelloAudioUnitRender::start()
{
    setAudioSession();
    setIOUnit();
    if (audioUnit) {
        OSStatus ret = AudioOutputUnitStart(audioUnit);
        return ret == noErr;
    }
    
    logger.i("HelloAudioUnitRender::start");
    return false;
}

int32_t HelloAudioUnitRender::getBytesPerFrame()
{
    return bytesPerFrame;
}

void HelloAudioUnitRender::stop()
{
    if (audioUnit) {
        AudioOutputUnitStop(audioUnit);
        AudioUnitUninitialize(audioUnit);
        AudioComponentInstanceDispose(audioUnit);
    }
    logger.i("HelloAudioUnitRender::stop");
}


OSStatus HelloAudioUnitRender::onAudioPlayCallbackFunc(void *inRefCon,
                                        AudioUnitRenderActionFlags *ioActionFlags,
                                        const AudioTimeStamp *inTimeStamp,
                                        UInt32 inBusNumber,
                                        UInt32 inNumberFrames,
                                        AudioBufferList *ioData)
{
    HelloAudioUnitRender *native = reinterpret_cast<HelloAudioUnitRender*>(inRefCon);
    
    void* buffer = ioData->mBuffers[0].mData;
    UInt32 bufferSize = ioData->mBuffers[0].mDataByteSize;
    
    
    if (native->callback) {
        native->callback(native, buffer, inNumberFrames, native->userdata);
    } else
    {
        // 默认静音
        memset(buffer, 0x00, bufferSize);
    }
    
    return noErr;
}
