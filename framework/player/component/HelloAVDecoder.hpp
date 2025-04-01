//
// Created by liaohailong on 2025/1/11.
//

#ifndef HELLOPLAYER_HELLOAVDECODER_HPP
#define HELLOPLAYER_HELLOAVDECODER_HPP

#include "HelloProcessor.hpp"

#include "../entity/IAVPacket.hpp"
#include "../entity/IAVFrame.hpp"
#include "../entity/AVProperties.hpp"
#include "../entity/PlayConfig.hpp"
#include "../entity/IAVMediaType.hpp"
#include "../HelloAVSync.hpp"
#include "../device/HelloDeviceRegister.hpp"
#include "../device/HelloAVDecoderFFmpeg.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavcodec/bsf.h>
}


/**
 * create by liaohailong
 * 2025/1/11 16:06
 * desc: 音频解码模块
 */
class HelloAVDecoder : public HelloProcessor<IAVPacket, IAVFrame>
{
public:
    explicit HelloAVDecoder(const char *tag, std::shared_ptr<HelloAVSync> avSync);

    ~HelloAVDecoder() override;

    void prepare(AVStream *stream, PlayConfig config);

    /**
     * 清空GOP中的所有参考序列帧
     */
    void flushBuffer();

    inline bool isPrepared() const noexcept
    {
        return prepared;
    };

    std::shared_ptr<AudioProperties> getAudioProperties();

    std::shared_ptr<VideoProperties> getVideoProperties();

protected:
    bool onProcess(std::shared_ptr<InputDataCtx> inputData) override;
    void onReset() override;

private:
    bool initBsf(AVStream *stream);
    void releaseBsf();
    static void onReceiveFrameCallback(std::shared_ptr<IAVFrame> frame, void *userdata);

private:

    /**
     * 同步器
     */
    std::shared_ptr<HelloAVSync> avSync;

    /**
     * 播放配置
     */
    PlayConfig config;
    /**
     * 是否已经准备好
     */
    std::atomic<bool> prepared;

    /**
     * 外部线程与内部handler操作相应需要同步
     */
    std::mutex mutex;

    /**
     * 轨道时间基
     */
    AVRational pkt_timebase;

    /**
     * 轨道类型
     */
    AVMediaType codec_type;

    /**
     * 音视频解码器
     */
    std::shared_ptr<HelloAVDecoderCore> decoder;
    
    /**
     * 比特流过滤器，用作h264或h265
     */
    AVBSFContext *bsf_ctx;

    /**
     * 媒体数据类型
     */
    IAVMediaType type;

    /**
     * 序列号，用来区分包是否过期，或则无效，默认从0开始
     */
    int64_t serial;

};


#endif //HELLOPLAYER_HELLOAVDECODER_HPP
