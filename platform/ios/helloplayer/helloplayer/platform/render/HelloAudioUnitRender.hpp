//
//  HelloAudioUnitRender.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/21.
//

#ifndef HelloAudioUnitRender_hpp
#define HelloAudioUnitRender_hpp


#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVAudioSession.h>


#include "HelloAudioRender.hpp"



extern "C"
{
#include <libavutil/samplefmt.h>
}

class HelloAudioUnitRender : public HelloAudioRender {
public:
    explicit HelloAudioUnitRender();
    ~HelloAudioUnitRender() override;
    
    std::shared_ptr<AudioProperties> setConfig(std::shared_ptr<AudioProperties> properties) override;

    void setFramesPerCallback(int framesPerCallback) override;

    void setCallback(Callback callback, void *userdata) override;

    bool start() override;

    int32_t getBytesPerFrame() override;

    void stop() override;
    
private:
    void setAudioSession();
    void setIOUnit();
    static OSStatus onAudioPlayCallbackFunc(void *inRefCon,
                                            AudioUnitRenderActionFlags *ioActionFlags,
                                            const AudioTimeStamp *inTimeStamp,
                                            UInt32 inBusNumber,
                                            UInt32 inNumberFrames,
                                            AudioBufferList *ioData);
    
private:
    UInt32 sampleRate; // 采样率
    UInt32 channelsPerFrame; // 声道数
    UInt32 bitsPerChannel; // 位深
    UInt32 bytePerSample; // 一个样本多少字节  = (bits/8)
    UInt32 bytesPerFrame; // 一个音频帧多少字节  = bytePerSample * channelsPerFrame
    AudioFormatFlags    formatFlags;
    int framesPerCallback; // 一次回调多少个样本
    
    AudioUnit audioUnit;

    Callback callback;
    void* userdata;
};

#endif /* HelloAudioUnitRender_hpp */
