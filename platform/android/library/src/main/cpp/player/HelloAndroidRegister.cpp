//
// Created by 廖海龙 on 2025/3/3.
//


// HelloPlayer通用层代码
#include "framework/player/HelloPlayer.hpp"
#include "framework/player/device/HelloDeviceRegister.hpp"
#include "framework/player/device/HelloAVDecoderFFmpeg.hpp"

// Android平台实现
#include "../render/HelloOpenGLESRender.hpp"
#include "../render/HelloOboAudioRender.hpp"
#include "../player/HelloAndroidCallback.hpp"

/**
 * @return 音视频解码器
 */
std::shared_ptr<HelloAVDecoderCore> HelloDeviceRegister::onCreateHelloAVDecoderCore()
{
    return std::make_shared<HelloAVDecoderFFmpeg>();
}

/**
 * @param format 像素格式
 * @return Android平台侧实现的视频渲染器
 */
std::shared_ptr<HelloVideoRender> HelloDeviceRegister::onCreateHelloVideoRender(AVPixelFormat format)
{
    return std::make_shared<HelloOpenGLESRender>(format);
}

/**
 * @return Android平台侧实现的音频渲染器
 */
std::shared_ptr<HelloAudioRender> HelloDeviceRegister::onCreateHelloAudioRender()
{
    return std::make_shared<HelloOboAudioRender>();
}

/**
 * @param userdata 携带参数
 * @return Android平台侧实现的播放器回调
 */
std::shared_ptr<HelloPlayerCallback> HelloPlayerCallback::createHelloPlayerCallback(void* userdata)
{
    return std::make_shared<HelloAndroidCallback>(userdata);
}
