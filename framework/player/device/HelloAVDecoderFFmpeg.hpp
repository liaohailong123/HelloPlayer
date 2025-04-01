//
// Created by liaohailong on 2025/3/15.
//

#ifndef HELLOPLAYER_HELLOAVDECODERFFMPEG_HPP
#define HELLOPLAYER_HELLOAVDECODERFFMPEG_HPP

#include "HelloAVDecoderCore.hpp"
#include "../../util/TimeUtil.hpp"

extern "C"
{
#include <libavcodec/avcodec.h>
}

#ifdef __ANDROID__
#define HELLO_HARDWARE_H264_CODEC_NAME "h264_mediacodec"
#define HELLO_HARDWARE_HEVC_CODEC_NAME "hevc_mediacodec"
#define HELLO_HARDWARE_AV1_CODEC_NAME "av1_mediacodec"
#define HELLO_HARDWARE_MPEG2_CODEC_NAME "mpeg2_mediacodec"
#define HELLO_HARDWARE_MPEG4_CODEC_NAME "mpeg4_mediacodec"
#define HELLO_HARDWARE_VP8_CODEC_NAME "vp8_mediacodec"
#define HELLO_HARDWARE_VP9_CODEC_NAME "vp9_mediacodec"
#endif

/**
 * create by liaohailong
 * 2025/3/15 14:04
 * desc: FFmpeg实现音视频解码器
 */
class HelloAVDecoderFFmpeg : public HelloAVDecoderCore
{
public:
    explicit HelloAVDecoderFFmpeg();

    ~HelloAVDecoderFFmpeg() override;

    bool prepare(AVStream *stream, PlayConfig config) override;

    /**
     * 清空GOP中的所有参考序列帧
     */
    bool flushBuffer() override;

    std::shared_ptr<AudioProperties> getAudioProperties() override;

    std::shared_ptr<VideoProperties> getVideoProperties() override;

    /**
     * @param pkt 编码数据
     * @return 参考 avcodec_send_packet 注解
     */
    int sendPacket(std::shared_ptr<IAVPacket> packet) override;

    void release() override;

private:
    AVPixelFormat find_hw_pix_fmt(const AVCodec *decoder);
    static AVPixelFormat get_hw_format(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts);
    static int hw_decoder_init(AVCodecContext *ctx, const enum AVHWDeviceType type);
private:
    /**
     * 解码器上下文
     */
    AVCodecContext *codec_ctx;

    /**
     * VideoToolbox 硬件解码相关
     */
    AVPixelFormat hw_pix_fmt;
    
    /**
     * VideoToolbox 硬件解码相关,从GPU拷贝数据到CPU上
     */
    AVBufferRef *hw_device_ctx;
};


#endif //HELLOPLAYER_HELLOAVDECODERFFMPEG_HPP
