//
// Created by liaohailong on 2025/1/11.
//

#include "HelloAVDecoder.hpp"


HelloAVDecoder::HelloAVDecoder(const char *tag, std::shared_ptr<HelloAVSync> avSync)
        : HelloProcessor(tag), prepared(false), onBeforeDecodeCallbackCtx(nullptr), mutex(),
          config(),
          pkt_timebase(), codec_type(AVMEDIA_TYPE_UNKNOWN), decoder(nullptr), bsf_ctx(nullptr),
          avSync(std::move(avSync)), type(IAVMediaType::UNKNOWN), serial(0)
{
    logger.i("HelloAVDecoder::HelloAVDecoder(%p)", this);
}

HelloAVDecoder::~HelloAVDecoder()
{
    reset();

    logger.i("HelloAVDecoder::HelloAVDecoder)", this);
}

void HelloAVDecoder::prepare(AVStream *stream, PlayConfig _config)
{
    // 修改资源操作，需挂锁
    std::unique_lock<std::mutex> locker(mutex);

    this->config = _config;

    if (!stream)
    {
        logger.i("doPrepare stream is nullptr");
        return;
    }

    if (!stream->codecpar)
    {
        logger.i("doPrepare stream->codecpar is nullptr");
        return;
    }

    pkt_timebase = stream->time_base;
    codec_type = stream->codecpar->codec_type;

    // 如果是视频解码，优先使用平台侧提供的解码器
    if (codec_type == AVMEDIA_TYPE_VIDEO && config.useHardware)
    {
        logger.i("try to use platform[hardware] decoder");
        decoder = HelloDeviceRegister::onCreateHelloAVDecoderCore();
        prepared = decoder->prepare(stream, config);
    }

    // 兜底：使用ffmpeg解码器
    if (!prepared)
    {
        if (config.useHardware)
        {
            logger.i("try to use ffmpeg[hardware] decoder");
        } else
        {
            logger.i("try to use ffmpeg[software] decoder");
        }
        decoder = std::make_shared<HelloAVDecoderFFmpeg>();
        prepared = decoder->prepare(stream, config);
        logger.i("to use ffmpeg decoder");
    }

    if (!prepared)
    {
        logger.i("prepared failure");
        return;
    }

    // 初始化 bsf 对 H.26X 系列视频编码处理
    if (decoder->needAnnexB())
    {
        initBsf(stream);
    }

    // 注册回调函数，接收解码好的视频帧
    decoder->setReceiveFrameCallback(onReceiveFrameCallback, this);

    logger.i("prepared success");
}


void HelloAVDecoder::setOnDecodeCallbackCtx(const std::shared_ptr<OnDecodeCallbackCtx> &ctx)
{
    this->onBeforeDecodeCallbackCtx = ctx;
}

/**
 * 清空GOP中的所有参考序列帧
 */
void HelloAVDecoder::flushBuffer()
{
    if (decoder)
    {
        decoder->flushBuffer();
        logger.i("decoder->flushBuffer()");
    }
}

std::shared_ptr<AudioProperties> HelloAVDecoder::getAudioProperties()
{
    if (decoder)
    {
        return decoder->getAudioProperties();
    }
    return std::make_shared<AudioProperties>();
}

std::shared_ptr<VideoProperties> HelloAVDecoder::getVideoProperties()
{
    if (decoder)
    {
        return decoder->getVideoProperties();
    }
    return std::make_shared<VideoProperties>();
}

bool HelloAVDecoder::onProcess(std::shared_ptr<InputDataCtx> inputData)
{

    // 修改资源操作，需挂锁,onReset函数已经持有锁,这里就终止操作
    if (!mutex.try_lock())
    {
        return false;
    }
    std::unique_lock<std::mutex> locker(mutex, std::adopt_lock);


    if (!decoder)
    {
        logger.i("decoder is nullptr?");
        return false;
    }

    // 媒体类型
    const char *mediaDesc = av_get_media_type_string(codec_type);

//    logger.i("decoder[%s] onProcess start", mediaDesc);

    HelloClock &masterClock = avSync->getMasterClock();
    std::shared_ptr<IAVPacket> &data = inputData->data;

    if (data->serial != masterClock.serial)
    {
        // 丢弃过期包
        return true;
    }

    // 判断是否为空包
    if (data->flushBuffers)
    {
        flushBuffer();
        logger.i("Victor decoder[%s] flush buffer", mediaDesc);
        return true;
    }

    // 数据结束标记位
    if (data->eof)
    {
        std::shared_ptr<IAVFrame> output = std::make_shared<IAVFrame>();
        // 设置媒体类型，后续流程会用到
        output->type = data->type;
        // 设置序列号
        output->serial = data->serial;

        // 手动添加 time_base
        output->frame->time_base = pkt_timebase;

        // End of file 标记位，告诉渲染器，方便后续有相关逻辑处理
        output->eof = true;
        // 数据回调
        sendCallback(output);

        logger.i("decoder[%s] receive eof", mediaDesc);
        return true;
    }

//    logger.i("decoder[%s] sendPacket pts[%d]ms serial[%d]",
//             mediaDesc, data->getPtsUs() / 1000, data->serial);

    // 判断一下是否需要延迟送解，主要是兼容一些低内存设备，防止内存爆了
    if (onBeforeDecodeCallbackCtx && onBeforeDecodeCallbackCtx->callback)
    {
        bool ret = onBeforeDecodeCallbackCtx->callback(data, onBeforeDecodeCallbackCtx->userdata);
        if (ret)
        {
            int64_t durationUs = data->getDurationUs();
            auto delayUs = int64_t(double(durationUs)/ masterClock.speed);
//            logger.i("Decoder[%s] delay [%d]ms to send packet",
//                     data->getMediaTypeDesc(), delayUs / 1000);
            callMeLater(delayUs);
            return false;
        }
    }

    // 保存下来，后面回调接口(onReceiveFrameCallback)有用到
    this->type = data->type;
    this->serial = data->serial;
    int64_t ptsUs = data->getPtsUs();

    int ret = 0;
    // MP4 模式 -> AnnexB 模式
    if (bsf_ctx)
    {
        ret = av_bsf_send_packet(bsf_ctx, data->packet);
        if (ret < 0)
        {
            FFUtil::av_print_error(logger, ret, "av_bsf_send_packet error");
            return true;
        }
        while (true)
        {
            std::shared_ptr<IAVPacket> copy = std::make_shared<IAVPacket>();
            copy->type = this->type;
            copy->serial = serial;
            ret = av_bsf_receive_packet(bsf_ctx, copy->packet);
            if (ret != 0)
            {
                break;
            }
            // 这里必须做到send一次，receive多次，因为这个packet没法去保存，如果send不进去的话
            ret = decoder->sendPacket(copy);
        }
    } else
    {
        // 这里必须做到send一次，receive多次，因为这个packet没法去保存，如果send不进去的话
        ret = decoder->sendPacket(data);
    }

    if (ret < 0)
    {
        FFUtil::av_print_error(logger, ret, "avcodec_send_packet error");
        return true; // 这里应该是不会存在 EAGAIN 的情况，因为都是send一次，receive多次
    }

//    logger.i("Victor decoder[%s] onProcess over pts[%d]ms serial[%d]",
//             mediaDesc, ptsUs / 1000, data->serial);
    return true;
}

bool HelloAVDecoder::initBsf(AVStream *stream)
{
    releaseBsf();
    if (!stream)
    {
        return false;
    }
    AVCodecID codec_id = stream->codecpar->codec_id;

    const char *bsf_h264 = "h264_mp4toannexb";
    const char *bsf_h265 = "hevc_mp4toannexb";
    const char *bsf_h266 = "vvc_mp4toannexb";

    const char *name = nullptr;
    if (codec_id == AV_CODEC_ID_H264)
    {
        name = bsf_h264;
    } else if (codec_id == AV_CODEC_ID_H265)
    {
        name = bsf_h265;
    } else if (codec_id == AV_CODEC_ID_H266)
    {
        name = bsf_h266;
    }

    if (!name)
    {
        return false;
    }

    const AVBitStreamFilter *bsfilter = av_bsf_get_by_name(name);
    int ret = av_bsf_alloc(bsfilter, &bsf_ctx);
    if (ret < 0 || !bsf_ctx)
    {
        return false;
    }
    // 将流参数拷贝给过滤器，之后再初始化，过滤器才能知道如何工作
    ret = avcodec_parameters_copy(bsf_ctx->par_in, stream->codecpar);
    if (ret < 0)
    {
        av_bsf_free(&bsf_ctx);
        return false;
    }
    // 初始化过滤器，成功之后才能使用
    ret = av_bsf_init(bsf_ctx);
    if (ret < 0)
    {
        av_bsf_free(&bsf_ctx);
        return false;
    }
    logger.i("bsf init success");
    return true;
}

void HelloAVDecoder::releaseBsf()
{
    if (bsf_ctx)
    {
        av_bsf_free(&bsf_ctx);
        bsf_ctx = nullptr;
    }
}

void HelloAVDecoder::onReceiveFrameCallback(const std::shared_ptr<IAVFrame> &output, void *userdata)
{
    auto native = reinterpret_cast<HelloAVDecoder *>(userdata);
    Logger &logger = native->logger;
    const char *mediaDesc = av_get_media_type_string(native->codec_type);

    if (!native->prepared)
    {
        logger.i("receive_frame[%s] return cause: not prepared", mediaDesc);
        return;
    }

    // 设置媒体类型，后续流程会用到
    output->type = native->type;
    // 设置序列号
    output->serial = native->serial;
    // 手动添加 time_base
    output->frame->time_base = native->pkt_timebase;

    logger.i("receive_frame[%s] frame->pts[%d]ms", mediaDesc, output->getPtsUs() / 1000);

    // 数据回调
    native->sendCallback(output);
}

void HelloAVDecoder::onReset()
{
    // 修改资源操作，需挂锁
    std::unique_lock<std::mutex> locker(mutex);

    prepared = false;

    if (decoder)
    {
        decoder->release();
        decoder = nullptr;
    }
    releaseBsf();
}
