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
#include "../render/opengl/HelloOpenGLESRender.hpp"
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
    // 苹果生态中,图形API全面推Metal,优先使用Metal
    // 实际测试中发现,iOS上的OpenGLES渲染效率存在耗时高问题,另外接口支持不全
    if (ctx.config.renderApi == RenderApi::OPENGL_ES) {
        return std::make_shared<HelloOpenGLESRender>(ctx.format);
    } else if (ctx.config.renderApi == RenderApi::METAL) {
        return std::make_shared<HelloMetalRender>(ctx.format);
    }
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

