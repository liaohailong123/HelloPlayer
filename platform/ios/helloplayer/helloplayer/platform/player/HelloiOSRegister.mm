//
//  HelloiOSRegister.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/21.
//

// HelloPlayer通用层代码
#include "player/device/HelloDeviceRegister.hpp"
#include "player/device/HelloAVDecoderFFmpeg.hpp"


// iOS平台实现
#include "../render/metal/HelloMetalRender.hpp"
#include "../render/HelloAudioUnitRender.hpp"
#include "HelloiOSCallback.hpp"

// Metal官方示例代码:
// https://developer.apple.com/documentation/metal/metal-sample-code-library?language=objc

/**
 * @return 音视频解码器
 */
std::shared_ptr<HelloAVDecoderCore> HelloDeviceRegister::onCreateHelloAVDecoderCore()
{
    return std::make_shared<HelloAVDecoderFFmpeg>();
    
}

/**
 * @param ctx 渲染配置上下文
 * @return Android平台侧实现的视频渲染器
 */
std::shared_ptr<HelloVideoRender> HelloDeviceRegister::onCreateHelloVideoRender(const HelloVideoRender::VideoRenderCtx &ctx)
{
    return std::make_shared<HelloMetalRender>(ctx.format);
}

/**
 * @return Android平台侧实现的音频渲染器
 */
std::shared_ptr<HelloAudioRender> HelloDeviceRegister::onCreateHelloAudioRender()
{
    return std::make_shared<HelloAudioUnitRender>();
}

/**
 * @param userdata 携带参数
 * @return 鸿蒙平台侧实现的播放器回调
 */
std::shared_ptr<HelloPlayerCallback> HelloPlayerCallback::createHelloPlayerCallback(void* userdata)
{
    return std::make_shared<HelloiOSCallback>(userdata);
}

