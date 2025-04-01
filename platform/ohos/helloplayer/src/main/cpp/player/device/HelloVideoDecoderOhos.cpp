//
// Created on 2025/3/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "HelloVideoDecoderOhos.hpp"
#include "framework/util/TimeUtil.hpp"
#include <cerrno>
#include <cstdint>


HelloVideoDecoderOhos::HelloVideoDecoderOhos()
    : HelloAVDecoderCore("HelloVideoDecoderOhos"), name(nullptr), decoder(nullptr), time_base(), running(false),
      colorFormat(-1), bufferQueueMutex(), bufferQueueCond(), bufferQueue(), packetDurationMutex(), packetDurationMap(),
      picWidth(0), picHeight(0), widthStride(0), heightStride(0)
{
    logger.i("HelloVideoDecoderOhos::HelloVideoDecoderOhos(%p)", this);
}

HelloVideoDecoderOhos::~HelloVideoDecoderOhos()
{
    if (decoder)
    {
        OH_VideoDecoder_Flush(decoder);
        OH_VideoDecoder_Stop(decoder);
        OH_VideoDecoder_Destroy(decoder);
        decoder = nullptr;
    }
    logger.i("HelloVideoDecoderOhos::~HelloVideoDecoderOhos(%p)", this);
}


bool HelloVideoDecoderOhos::needAnnexB() 
{
    return true;
}

bool HelloVideoDecoderOhos::prepare(AVStream *stream, PlayConfig config)
{
    if (!stream)
    {
        logger.i("stream is null?");
        return false;
    }
    AVCodecID codec_id = stream->codecpar->codec_id;

    // https://developer.huawei.com/consumer/cn/doc/harmonyos-guides-V14/avcodec-support-formats-V14#%E8%A7%86%E9%A2%91%E8%A7%A3%E7%A0%81
    // 当前支持的解码类型：
    // 视频硬解类型	                        视频软解类型
    // AVC(H.264)、HEVC(H.265)、VVC(H.266)	AVC(H.264)、HEVC(H.265)

    if (AV_CODEC_ID_H264 == codec_id)
    {
        name = OH_AVCODEC_MIMETYPE_VIDEO_AVC;
    } else if (AV_CODEC_ID_HEVC == codec_id)
    {
        name = OH_AVCODEC_MIMETYPE_VIDEO_HEVC;
    } else if (AV_CODEC_ID_VVC == codec_id)
    {
        name = OH_AVCODEC_MIMETYPE_VIDEO_VVC;
    }

    if (!name)
    {
        logger.i("not support encode type[%d]", codec_id);
        return false;
    }

    decoder = OH_VideoDecoder_CreateByMime(name);

    // 第一步：Configure video decoder
    OH_AVFormat *format = OH_AVFormat_Create();

    int videoWidth = stream->codecpar->width;
    int videoHeight = stream->codecpar->height;
    // 或者通过 time_base 来计算 FPS
    double frameRate = 1.0 / av_q2d(stream->time_base);
    logger.i("FPS from time_base: %.2f", frameRate);
    time_base = stream->time_base;
    // 官方demo里面这个值是固定的，https://gitee.com/harmonyos_samples/AVCodecVideo
    OH_AVPixelFormat pixelFormat = AV_PIXEL_FORMAT_NV12; // NV12不好用,鸿蒙模拟器和Android的GL着色器,取UV数据有差异
//    OH_AVPixelFormat pixelFormat = AV_PIXEL_FORMAT_YUVI420; // 优先使用YUVI420 (真机 华为mate60不能用这个值)
    int32_t rotation = 0;
    int ROTATION_ANGLE = 90;

    OH_AVFormat_SetIntValue(format, OH_MD_KEY_WIDTH, videoWidth);
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_HEIGHT, videoHeight);
    OH_AVFormat_SetDoubleValue(format, OH_MD_KEY_FRAME_RATE, frameRate);
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_PIXEL_FORMAT, pixelFormat);
    if (videoHeight > videoWidth)
    {
        OH_AVFormat_SetIntValue(format, OH_MD_KEY_ROTATION, rotation + ROTATION_ANGLE);
    }

    int ret = OH_VideoDecoder_Configure(decoder, format);
    OH_AVFormat_Destroy(format);
    format = nullptr;

    if (ret != AV_ERR_OK)
    {
        logger.i("Config failed, ret: %d", ret);
        return false;
    }

    // 第二步：SetCallback for video decoder
    ret = OH_VideoDecoder_RegisterCallback(
        decoder, {OnCodecError, OnCodecFormatChange, OnNeedInputBuffer, OnNewOutputBuffer}, this);
    if (ret != AV_ERR_OK)
    {
        logger.i("Set callback failed, ret: %d", ret);
        return false;
    }

    // 第三步：Prepare video decoder
    ret = OH_VideoDecoder_Prepare(decoder);
    if (ret != AV_ERR_OK)
    {
        logger.i("Prepare failed, ret: %d", ret);
        return false;
    }

    running = true;
    // 第四步：Start video decoder
    ret = OH_VideoDecoder_Start(decoder);
    if (ret != AV_ERR_OK)
    {
        running = false;
        logger.i("Start failed, ret: %d", ret);
        return false;
    }

    logger.i("prepare success");
    return true;
}

/**
 * 清空GOP中的所有参考序列帧
 */
bool HelloVideoDecoderOhos::flushBuffer()
{
    if (decoder)
    {
        // 先清空解码容器buffer
        std::unique_lock<std::mutex> locker(bufferQueueMutex);
        while (!bufferQueue.empty())
            bufferQueue.pop();
        locker.unlock();

        // 清除 GOP
        OH_VideoDecoder_Flush(decoder);
        OH_VideoDecoder_Start(decoder);
        logger.i("ohos video decoder flush buffers");

        return true;
    }

    return false;
}

std::shared_ptr<AudioProperties> HelloVideoDecoderOhos::getAudioProperties()
{
    return std::make_shared<AudioProperties>();
}

std::shared_ptr<VideoProperties> HelloVideoDecoderOhos::getVideoProperties()
{
    std::shared_ptr<VideoProperties> p = std::make_shared<VideoProperties>();
    p->width = -1;
    p->height = -1;
    p->fps = -1;
    p->colorFormat = AV_PIX_FMT_NONE;

    if (decoder)
    {
        OH_AVFormat *format = OH_VideoDecoder_GetOutputDescription(decoder);
        if (format)
        {
            int32_t width = 0;
            int32_t height = 0;
            double framerate = 0;
            int32_t colorFormat = 0;
            OH_AVFormat_GetIntValue(format, OH_MD_KEY_WIDTH, &width);
            OH_AVFormat_GetIntValue(format, OH_MD_KEY_HEIGHT, &height);
            OH_AVFormat_GetDoubleValue(format, OH_MD_KEY_FRAME_RATE, &framerate);
            OH_AVFormat_GetIntValue(format, OH_MD_KEY_PIXEL_FORMAT, &colorFormat);
            p->width = width;
            p->height = height;
            p->fps = framerate;
            if (colorFormat == AV_PIXEL_FORMAT_NV12)
            {
                p->colorFormat = AV_PIX_FMT_NV12;
            } else if (colorFormat == AV_PIXEL_FORMAT_NV21)
            {
                p->colorFormat = AV_PIX_FMT_NV21;
            } else if (colorFormat == AV_PIXEL_FORMAT_YUVI420)
            {
                p->colorFormat = AV_PIX_FMT_YUV420P;
            } else
            {
                logger.i("video color format not found [%d]", colorFormat);
            }
        }
    }

    return p;
}

/**
 * @param pkt 编码数据
 * @return 参考 avcodec_send_packet 注解
 */
int HelloVideoDecoderOhos::sendPacket(std::shared_ptr<IAVPacket> packet)
{
    // 放进来的数据都需要处理掉
    return sendPacket2(packet);
}

int HelloVideoDecoderOhos::sendPacket2(std::shared_ptr<IAVPacket> packet)
{

    std::unique_lock<std::mutex> locker(bufferQueueMutex);
    bufferQueueCond.wait(locker, [this]() -> bool { return !this->running || !this->bufferQueue.empty(); });
    if (!running)
    {
        logger.i("decoder is not running... let us quite");
        return 0;
    }

    OhosBufferData data = bufferQueue.front();
    bufferQueue.pop();
    logger.i("bufferQueue.pop() bufferQueue.size[%d]", bufferQueue.size());
    locker.unlock(); // 提前放锁，给生产者加数据


    // 把数据从 packet 读到 buffer 上
    bool success = readSample(data.codec, data.index, data.buffer, packet, data.userData);

    if (!success)
    {
        logger.e("OnNeedInputBuffer readSample not success");
        return AVERROR(ENOMEM);
    }

    return 0;
}

/**
 * 释放解码器
 */
void HelloVideoDecoderOhos::release()
{
    running = false;
    bufferQueueCond.notify_all();
    logger.i("HelloVideoDecoderOhos::release");
}

void HelloVideoDecoderOhos::putPacketDuration(int64_t pts, int64_t duration)
{
    std::unique_lock<std::mutex> locker(packetDurationMutex);
    packetDurationMap[pts] = duration;
    return;
}

bool HelloVideoDecoderOhos::getPacketDuration(int64_t pts, int64_t *duration)
{
    std::unique_lock<std::mutex> locker(packetDurationMutex);
    auto it = packetDurationMap.find(pts);
    if (it != packetDurationMap.end())
    {
        *duration = it->second;
        packetDurationMap.erase(pts);
        return true;
    }
    return false;
}


void HelloVideoDecoderOhos::OnCodecError(OH_AVCodec *codec, int32_t errorCode, void *userData)
{
    auto native = reinterpret_cast<HelloVideoDecoderOhos *>(userData);
    Logger &logger = native->logger;
    logger.e("OnCodecError errorCode[%d]", errorCode);
}
void HelloVideoDecoderOhos::OnCodecFormatChange(OH_AVCodec *codec, OH_AVFormat *format, void *userData)
{
    auto native = reinterpret_cast<HelloVideoDecoderOhos *>(userData);
    Logger &logger = native->logger;
    int32_t width = 0;
    int32_t height = 0;
    double framerate = 0;
    int32_t colorFormat = 0;
    OH_AVFormat_GetIntValue(format, OH_MD_KEY_WIDTH, &width);
    OH_AVFormat_GetIntValue(format, OH_MD_KEY_HEIGHT, &height);
    OH_AVFormat_GetDoubleValue(format, OH_MD_KEY_FRAME_RATE, &framerate);
    OH_AVFormat_GetIntValue(format, OH_MD_KEY_PIXEL_FORMAT, &colorFormat);


    OH_AVFormat_GetIntValue(format, OH_MD_KEY_VIDEO_PIC_WIDTH, &native->picWidth);
    OH_AVFormat_GetIntValue(format, OH_MD_KEY_VIDEO_PIC_HEIGHT, &native->picHeight);
    OH_AVFormat_GetIntValue(format, OH_MD_KEY_VIDEO_STRIDE, &native->widthStride);
    OH_AVFormat_GetIntValue(format, OH_MD_KEY_VIDEO_SLICE_HEIGHT, &native->heightStride);
    native->colorFormat = colorFormat;
    logger.i("OnCodecFormatChange(%p) width[%d,%d] height[%d,%d] framerate[%f] colorFormat[%d] ", native, width,
             native->picWidth, height, native->picHeight, framerate, colorFormat);
}
void HelloVideoDecoderOhos::OnNeedInputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData)
{
    auto native = reinterpret_cast<HelloVideoDecoderOhos *>(userData);
    Logger &logger = native->logger;


    // 一旦有buffer可供外部解码，马上唤醒
    std::unique_lock<std::mutex> locker(native->bufferQueueMutex);
    OhosBufferData data = {codec, index, buffer, userData};
    native->bufferQueue.push(data);
    logger.i("bufferQueue.push(data) bufferQueue.size[%d]", native->bufferQueue.size());
    locker.unlock();
    native->bufferQueueCond.notify_all();
}

void HelloVideoDecoderOhos::OnNewOutputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData)
{
    auto native = reinterpret_cast<HelloVideoDecoderOhos *>(userData);
    Logger &logger = native->logger;
    int64_t startMs = TimeUtil::getCurrentTimeMs();

    // 帮我把 OH_AVBuffer *buffer 中的内容，拷贝到 frame 中
    std::shared_ptr<IAVFrame> frame = std::make_shared<IAVFrame>();

    // 把数据从 buffer 读到 frame 上
    bool success = writeSample(codec, index, buffer, frame, userData);

    if (!success)
    {
        logger.e("OnNewOutputBuffer writeSample not success");
        return;
    }

    OH_VideoDecoder_FreeOutputBuffer(codec, index);

    int64_t costMs = TimeUtil::getCurrentTimeMs() - startMs;
    if (costMs > 5)
    {
        logger.i("OnNewOutputBuffer end index[%d] cost[%d]ms", index, costMs);
    }

    // 通知外部
    if (native->callback)
    {
        native->callback(frame, native->userdata);
    }
}


bool HelloVideoDecoderOhos::readSample(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer,
                                       std::shared_ptr<IAVPacket> packet, void *userData)
{
    int64_t startMs = TimeUtil::getCurrentTimeMs();
    auto native = reinterpret_cast<HelloVideoDecoderOhos *>(userData);
    Logger &logger = native->logger;


    uint8_t *addr = OH_AVBuffer_GetAddr(buffer);
    int32_t capacity = OH_AVBuffer_GetCapacity(buffer);


    int size = packet->packet->size;
    if (capacity < size)
    {
        logger.e("readSample capacity[%d] < size[%d]", capacity, size);
        return false;
    }
    uint8_t *frameData = packet->packet->data;
    memcpy(addr, frameData, size);
    // 配置帧数据的输入尺寸、偏移量、时间戳等字段信息
    OH_AVCodecBufferAttr info;
    info.size = size;
    info.offset = 0;
    info.pts = packet->getPtsUs();
    native->putPacketDuration(info.pts, packet->getDurationUs());

    if (packet->packet->flags & AV_PKT_FLAG_KEY)
    {
        info.flags = AVCODEC_BUFFER_FLAGS_SYNC_FRAME;
    } else
    {
        info.flags = AVCODEC_BUFFER_FLAGS_NONE;
    }
    // info信息写入buffer
    int ret = OH_AVBuffer_SetBufferAttr(buffer, &info);
    if (ret != AV_ERR_OK)
    {
        logger.e("readSample OH_AVBuffer_SetBufferAttr ret[%d] != AV_ERR_OK", ret);
        return false;
    }

    logger.i("readSample info.pts[%ld]", info.pts);

    // 送入解码输入队列进行解码，index为对应buffer队列的下标
    ret = OH_VideoDecoder_PushInputBuffer(codec, index);
    if (ret != AV_ERR_OK)
    {
        logger.e("OH_VideoDecoder_PushInputBuffer ret[%d] != AV_ERR_OK", ret);
        return false;
    }

    int64_t costMs = TimeUtil::getCurrentTimeMs() - startMs;
    if (costMs > 5)
    {
        logger.i("OnNeedInputBuffer end index[%d] cost[%d]ms", index, costMs);
    }

    return true;
}

bool HelloVideoDecoderOhos::writeSample(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer,
                                        std::shared_ptr<IAVFrame> frame, void *userData)
{
    auto native = reinterpret_cast<HelloVideoDecoderOhos *>(userData);
    Logger &logger = native->logger;

    frame->frame->width = native->picWidth;
    frame->frame->height = native->picHeight;
    if (native->colorFormat == AV_PIXEL_FORMAT_YUVI420)
    {
        frame->frame->format = AV_PIX_FMT_YUV420P;
    } else if (native->colorFormat == AV_PIXEL_FORMAT_NV12)
    {
        frame->frame->format = AV_PIX_FMT_NV12;
    } else if (native->colorFormat == AV_PIXEL_FORMAT_NV21)
    {
        frame->frame->format = AV_PIX_FMT_NV21;
    } else
    {
        frame->frame->format = AV_PIX_FMT_NONE;
        logger.e("OnNewOutputBuffer error colorFormat[%d]", native->colorFormat);
        OH_VideoDecoder_FreeOutputBuffer(codec, index);
        return false;
    }

    int ret = av_frame_get_buffer(frame->frame, 1);
    if (ret != 0)
    {
        logger.e("OnNewOutputBuffer av_frame_get_buffer ret[%d]", ret);
        OH_VideoDecoder_FreeOutputBuffer(codec, index);
        return false;
    }

    uint8_t *addr = OH_AVBuffer_GetAddr(buffer);

    // 根据srcRect（跨距）和dstRect（资源宽高），把addr数据合理的拷贝到 frame->frame 中去
    int32_t dstW = native->picWidth;
    int32_t dstH = native->picHeight;
    int32_t srcStride = native->widthStride;
    int32_t srcH = native->heightStride;

    // 5️⃣ 拷贝 YUV 数据
    if (frame->frame->format == AV_PIX_FMT_YUV420P)
    {
        uint8_t *srcY = addr;
        uint8_t *srcU = addr + srcStride * srcH;
        uint8_t *srcV = srcU + (srcStride / 2) * (srcH / 2);

        uint8_t *dstY = frame->frame->data[0];
        uint8_t *dstU = frame->frame->data[1];
        uint8_t *dstV = frame->frame->data[2];

        // 逐行拷贝 Y 分量
        for (int i = 0; i < dstH; i++)
        {
            memcpy(dstY + i * frame->frame->linesize[0], srcY + i * srcStride, dstW);
        }

        // 逐行拷贝 U / V 分量
        for (int i = 0; i < dstH / 2; i++)
        {
            memcpy(dstU + i * frame->frame->linesize[1], srcU + i * (srcStride / 2), dstW / 2);
            memcpy(dstV + i * frame->frame->linesize[2], srcV + i * (srcStride / 2), dstW / 2);
        }
    } else if (frame->frame->format == AV_PIX_FMT_NV12 || frame->frame->format == AV_PIX_FMT_NV21)
    {
        uint8_t *srcY = addr;
        uint8_t *srcUV = addr + srcStride * srcH;

        uint8_t *dstY = frame->frame->data[0];
        uint8_t *dstUV = frame->frame->data[1];

        // 逐行拷贝 Y 分量
        for (int i = 0; i < dstH; i++)
        {
            memcpy(dstY + i * frame->frame->linesize[0], srcY + i * srcStride, dstW);
        }

        // 逐行拷贝 UV 分量
        for (int i = 0; i < dstH / 2; i++)
        {
            memcpy(dstUV + i * frame->frame->linesize[1], srcUV + i * srcStride, dstW);
        }
    }

    // 补充: pts duration time_base
    OH_AVCodecBufferAttr attr;
    OH_AVErrCode code = OH_AVBuffer_GetBufferAttr(buffer, &attr);
    if (code != AV_ERR_OK)
    {
        logger.e("OnNewOutputBuffer OH_AVBuffer_GetBufferAttr code[%d] != AV_ERR_OK", code);
        OH_VideoDecoder_FreeOutputBuffer(codec, index);
        return false;
    }
    // 注意时间基转换
    int64_t pts = av_rescale_q(attr.pts, (AVRational){1, 1000000}, native->time_base);
    // 或者通过 time_base 来计算 FPS
    double frameRate = 1.0 / av_q2d(native->time_base);
    int64_t durationUs = frameRate * 1000000;
    if (!native->getPacketDuration(attr.pts, &durationUs))
    {
        logger.i("getPacketDuration(%d) error", attr.pts);
    }
    logger.i("OnNewOutputBuffer attr.pts[%ld] durationUs[%d]", attr.pts, durationUs);
    int64_t duration = av_rescale_q(durationUs, (AVRational){1, 1000000}, native->time_base);

    frame->frame->pts = pts;
    frame->frame->duration = duration;
    frame->frame->flags = 0;
    if (attr.flags == AVCODEC_BUFFER_FLAGS_SYNC_FRAME)
    {
        frame->frame->flags = AV_FRAME_FLAG_KEY;
    }

    return true;
}