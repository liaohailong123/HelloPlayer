//
// Created by liaohailong on 2025/3/15.
//

#include "HelloAVDecoderFFmpeg.hpp"

HelloAVDecoderFFmpeg::HelloAVDecoderFFmpeg() : HelloAVDecoderCore("HelloAVDecoderFFmpeg"),
                                codec_ctx(nullptr), hw_pix_fmt(AV_PIX_FMT_NONE), hw_device_ctx(nullptr)
{
    logger.i("HelloAVDecoderFFmpeg::HelloAVDecoderFFmpeg(%p)", this);
}

HelloAVDecoderFFmpeg::~HelloAVDecoderFFmpeg()
{
    logger.i("HelloAVDecoderFFmpeg::~HelloAVDecoderFFmpeg(%p)", this);
}

bool HelloAVDecoderFFmpeg::prepare(AVStream *_stream, PlayConfig _config)
{
    this->stream = _stream;
    this->config = _config;

    int ret;
    AVCodec *codec = nullptr;

    logger.i("stream->codecpar->codec_id[%s]", avcodec_get_name(stream->codecpar->codec_id));

#ifdef __ANDROID__
    // 尝试使用硬件解码
    if (config.useHardware)
    {
        std::string hwDecoderName;
        if (stream->codecpar->codec_id == AV_CODEC_ID_H264)
        {
            hwDecoderName = HELLO_HARDWARE_H264_CODEC_NAME;
        } else if (stream->codecpar->codec_id == AV_CODEC_ID_HEVC)
        {
            hwDecoderName = HELLO_HARDWARE_HEVC_CODEC_NAME;
        } else if (stream->codecpar->codec_id == AV_CODEC_ID_AV1)
        {
            hwDecoderName = HELLO_HARDWARE_AV1_CODEC_NAME;
        } else if (stream->codecpar->codec_id == AV_CODEC_ID_MPEG2TS)
        {
            hwDecoderName = HELLO_HARDWARE_MPEG2_CODEC_NAME;
        } else if (stream->codecpar->codec_id == AV_CODEC_ID_MPEG4)
        {
            hwDecoderName = HELLO_HARDWARE_MPEG4_CODEC_NAME;
        } else if (stream->codecpar->codec_id == AV_CODEC_ID_VP8)
        {
            hwDecoderName = HELLO_HARDWARE_VP8_CODEC_NAME;
        } else if (stream->codecpar->codec_id == AV_CODEC_ID_VP9)
        {
            hwDecoderName = HELLO_HARDWARE_VP9_CODEC_NAME;
        }

        if (!hwDecoderName.empty())
        {
            const AVCodec *hw_codec = avcodec_find_decoder_by_name(hwDecoderName.c_str());
            if (hw_codec)
            {
                logger.i("use hardware accelerate name[%s]", hwDecoderName.c_str());
            }
            codec = const_cast<AVCodec *>(hw_codec);
        }
    }
#endif

    // 继续使用软解码器
    if (!codec)
    {
        const AVCodec *sw_codec = avcodec_find_decoder(stream->codecpar->codec_id);
        codec = const_cast<AVCodec *>(sw_codec);
    }

    if (!codec)
    {
        logger.i("avcodec_find_decoder[%d] error", stream->codecpar->codec_id);
        return false;
    }

#ifdef __APPLE__
    if (config.useHardware)
    {

        // 视频解码尝试使用 VideoToolBox 做硬件加速
        bool isVideo = this->stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO;
        if (isVideo)
        {
            hw_pix_fmt = find_hw_pix_fmt(codec);
        }
    }
#endif

    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx)
    {
        logger.i("avcodec_alloc_context3 error");
        return false;
    }

    ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
    if (ret < 0)
    {
        logger.i("Failed to copy %s codec parameters to decoder context",
                 av_get_media_type_string(stream->codecpar->codec_type));
        return false;
    }

#ifdef __APPLE__
    if (hw_pix_fmt != AV_PIX_FMT_NONE) {
        codec_ctx->opaque = this;
        codec_ctx->get_format = get_hw_format;
        ret = hw_decoder_init(codec_ctx, AV_HWDEVICE_TYPE_VIDEOTOOLBOX);
        if (ret < 0)
        {
            FFUtil::av_print_error(logger, ret, "videotoolbox decode configure error");
            // 硬解不行,就继续用软解
        } else
        {
            logger.i("hw_decode_init success using videotoolbox");
        }
    }
#endif

    
    ret = avcodec_open2(codec_ctx, codec, nullptr);
    if (ret < 0)
    {
        FFUtil::av_print_error(logger, ret, "avcodec_open2 error");
        return false;
    }
    
    
    // 设置pkt_timebase
    codec_ctx->pkt_timebase = stream->time_base;
    codec_ctx->codec_id = codec->id;

    logger.i("prepared success");
    return true;
}

/**
 * 清空GOP中的所有参考序列帧
 */
bool HelloAVDecoderFFmpeg::flushBuffer()
{
    if (codec_ctx)
    {
        avcodec_flush_buffers(codec_ctx);
        logger.i("avcodec_flush_buffers");
        return true;
    }
    return false;
}

std::shared_ptr<AudioProperties> HelloAVDecoderFFmpeg::getAudioProperties()
{
    std::shared_ptr<AudioProperties> p = std::make_shared<AudioProperties>();
    if (codec_ctx)
    {
        p->sampleRate = codec_ctx->sample_rate;
        p->channelCount = codec_ctx->ch_layout.nb_channels;
        p->sampleFmt = codec_ctx->sample_fmt;
        p->frame_size = codec_ctx->frame_size;
        logger.i("getAudioProperties sampleFmt[%d]", codec_ctx->sample_fmt);
    }

    return p;
}

std::shared_ptr<VideoProperties> HelloAVDecoderFFmpeg::getVideoProperties()
{
    std::shared_ptr<VideoProperties> p = std::make_shared<VideoProperties>();
    if (codec_ctx)
    {
        p->width = codec_ctx->width;
        p->height = codec_ctx->height;
        p->fps = codec_ctx->framerate.den;
        p->colorFormat = codec_ctx->pix_fmt;
        if (hw_pix_fmt == AV_PIX_FMT_VIDEOTOOLBOX)
        {
            p->colorFormat = AV_PIX_FMT_NV12;
        }
    }

    return p;
}

/**
 * @param packet 编码数据
 * @return 参考 avcodec_send_packet 注解
 */
int HelloAVDecoderFFmpeg::sendPacket(std::shared_ptr<IAVPacket> packet)
{

    if (codec_ctx != nullptr)
    {
        int ret = avcodec_send_packet(codec_ctx, packet->packet);
        if (ret < 0)
        {
            if (IAVMediaType::VIDEO == packet->type)
            {
                FFUtil::av_print_error(logger, ret, "avcodec_send_packet[video] error");
            } else if(IAVMediaType::AUDIO == packet->type)
            {
                FFUtil::av_print_error(logger, ret, "avcodec_send_packet[audio] error");
            }
            return true; // 这里应该是不会存在 EAGAIN 的情况，因为都是send一次，receive多次
        }

        AVFrame *frame = av_frame_alloc();
        AVFrame *sw_frame = av_frame_alloc();
        AVFrame *tmp_frame = nullptr;

        while (ret >= 0)
        {
            // 注释：
            // function will always call av_frame_unref(frame) before doing anything else.
            ret = avcodec_receive_frame(codec_ctx, frame);

            if (ret == AVERROR(EAGAIN))
            {
                // 视频：可能是遇到了 B 帧，然后需要继续推入一个P帧才能进行解码
                // 音频：可能真的是解码完了，没有编码数据需要解了，需要继续送
                logger.i("avcodec_receive_frame[EAGAIN]");
                goto end;
            } else if (ret == AVERROR_EOF)
            {
                logger.i("avcodec_receive_frame[AVERROR_EOF]");
                goto end;
            } else if (ret == AVERROR_DECODER_NOT_FOUND)
            {
                logger.i("avcodec_receive_frame[AVERROR_DECODER_NOT_FOUND]");
                goto end;
            }

            // 回调外部
            if (callback)
            {
                if (frame->format == hw_pix_fmt) {
//                    int64_t startMs = TimeUtil::getCurrentTimeMs();
                    // retrieve data from GPU to CPU
                    ret = av_hwframe_transfer_data(sw_frame, frame, 0);
                    if (ret < 0) {
                        logger.i("Error transferring the data to system memory");
                        goto end;
                    }
                    ret = av_frame_copy_props(sw_frame, frame);
                    if (ret < 0) {
                        logger.i("Error copy metadata fields");
                        goto end;
                    }
                    // iPad 1920*1080 1~2ms
//                    int64_t costMs = TimeUtil::getCurrentTimeMs() - startMs;
//                    logger.i("copy buffer from GPU to CPU cost[%d]ms", costMs);
                    tmp_frame = sw_frame;
                } else
                {
                    tmp_frame = frame;
                }
                std::shared_ptr<IAVFrame> output = std::make_shared<IAVFrame>();
                output->move(tmp_frame);
                callback(output, userdata);
            }
        }
        end:
        av_frame_free(&frame);
        av_frame_free(&sw_frame);
        return ret;
    }
    return AVERROR_DECODER_NOT_FOUND;
}

/**
 * 释放解码器
 */
void HelloAVDecoderFFmpeg::release()
{
    if (codec_ctx)
    {
        if (codec_ctx->hw_device_ctx) {
            av_buffer_unref(&codec_ctx->hw_device_ctx);
        }
        avcodec_free_context(&codec_ctx);
        codec_ctx = nullptr;
    }
}

AVPixelFormat HelloAVDecoderFFmpeg::find_hw_pix_fmt(const AVCodec *decoder)
{
    
    enum AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;
    while((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE)
    {
        logger.i("Available devices type: %s", av_hwdevice_get_type_name(type));
    }
    // 使用 videotoolbox 硬件解码
    type = AV_HWDEVICE_TYPE_VIDEOTOOLBOX;
    for(int i=0;;i++)
    {
        const AVCodecHWConfig *config = avcodec_get_hw_config(decoder, i);
        if (!config)
        {
            logger.e("Decoder does not support device type %s", av_hwdevice_get_type_name(type));
            break;
        }
        if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX
            && config->device_type == type) {
            hw_pix_fmt = config->pix_fmt;
            break;
        }
    }
    
    
    return hw_pix_fmt;
}

AVPixelFormat HelloAVDecoderFFmpeg::get_hw_format(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts)
{
    HelloAVDecoderFFmpeg *native = (HelloAVDecoderFFmpeg *)ctx->opaque;
    const enum AVPixelFormat *p;
    for (p = pix_fmts; *p != -1; p++) {
        if (*p == native->hw_pix_fmt) {
            return *p;
        }
    }
    return AV_PIX_FMT_NONE;
}

int HelloAVDecoderFFmpeg::hw_decoder_init(AVCodecContext *ctx, const enum AVHWDeviceType type)
{
    HelloAVDecoderFFmpeg *native = (HelloAVDecoderFFmpeg *)ctx->opaque;
    int err = 0;
    
    if ((err = av_hwdevice_ctx_create(&native->hw_device_ctx,type,nullptr,nullptr,0)) < 0)
    {
        return err;
    }
    
    ctx->hw_device_ctx = av_buffer_ref(native->hw_device_ctx);
    return err;
}

