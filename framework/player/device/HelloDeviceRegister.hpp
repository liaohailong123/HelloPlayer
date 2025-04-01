//
// Created by liaohailong on 2025/2/27.
//

#ifndef HELLOPLAYER_HELLODEVICEREGISTER_HPP
#define HELLOPLAYER_HELLODEVICEREGISTER_HPP


#include "HelloVideoRender.hpp"
#include "HelloAudioRender.hpp"
#include "HelloAVDecoderCore.hpp"

/**
 * create by liaohailong
 * 2025/2/27 21:19
 * desc: 硬件功能注册器：音频播放，视频播放
 */
class HelloDeviceRegister
{

public:
    /**
     * @return 音视频解码器
     */
    static std::shared_ptr<HelloAVDecoderCore> onCreateHelloAVDecoderCore();
    /**
     * @param format 像素格式
     * @return 平台侧视频渲染器
     */
    static std::shared_ptr<HelloVideoRender> onCreateHelloVideoRender(AVPixelFormat format);
    /**
     * @return 平台侧音频渲染器
     */
    static std::shared_ptr<HelloAudioRender> onCreateHelloAudioRender();

};


#endif //HELLOPLAYER_HELLODEVICEREGISTER_HPP
