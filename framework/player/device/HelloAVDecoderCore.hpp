//
// Created by liaohailong on 2025/3/15.
//

#ifndef HELLOPLAYER_HELLOAVDECODERCORE_HPP
#define HELLOPLAYER_HELLOAVDECODERCORE_HPP

#include "../../log/Logger.hpp"

#include "../entity/IAVPacket.hpp"
#include "../entity/IAVFrame.hpp"
#include "../entity/AVProperties.hpp"
#include "../entity/PlayConfig.hpp"

extern "C"
{
#include <libavformat/avformat.h>
}

/**
 * create by liaohailong
 * 2025/3/15 13:46
 * desc: 处理音视频解码的核心组件
 */
class HelloAVDecoderCore
{
    typedef void (*OnReceiveFrameCallback)(const std::shared_ptr<IAVFrame> &frame, void *userdata);

public:
    explicit HelloAVDecoderCore(const char *tag);

    virtual ~HelloAVDecoderCore();
    
    /**
     * @return 需要
     */
    virtual bool needAnnexB();

    virtual bool prepare(AVStream *stream, const PlayConfig &config) = 0;

    /**
     * 清空GOP中的所有参考序列帧
     */
    virtual bool flushBuffer() = 0;

    virtual std::shared_ptr<AudioProperties> getAudioProperties() = 0;

    virtual std::shared_ptr<VideoProperties> getVideoProperties() = 0;

    /**
     * @param packet 编码数据
     * @return 参考 avcodec_send_packet 注解
     */
    virtual int sendPacket(std::shared_ptr<IAVPacket> packet) = 0;

    /**
     * @param callback 注册 解码帧完毕 回调
     * @param userdata 外部数据
     */
    void setReceiveFrameCallback(OnReceiveFrameCallback callback, void *userdata);

    /**
     * 释放解码器
     */
    virtual void release() = 0;


protected:
    Logger logger;

    /**
     * 轨道信息
     */
    AVStream *stream;

    /**
     * 播放配置
     */
    PlayConfig config;

    /**
     * 接收解码帧回调
     */
    OnReceiveFrameCallback callback;
    void *userdata;
};


#endif //HELLOPLAYER_HELLOAVDECODERCORE_HPP
