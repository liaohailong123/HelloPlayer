//
// Created by 廖海龙 on 2025/1/21.
//

#include "VideoConverter.hpp"

VideoConverter::VideoConverter(): logger("VideoConverter"), swsCtx(nullptr)
{
    logger.i("VideoConverter::VideoConverter(%p)", this);
}
VideoConverter::~VideoConverter()
{
    if (swsCtx)
    {
        sws_freeContext(swsCtx);
        swsCtx = nullptr;
    }
    logger.i("VideoConverter::~VideoConverter(%p)", this);
}

bool VideoConverter::convert(AVFrame *srcFrame, AVPixelFormat _dstFormat, AVFrame **_dstFrame)
{
    if (!srcFrame)
    {
        logger.i("Invalid source frame");
        return false;
    }

    // 类型改变了，重新创建sws上下文
    if (swsCtx && dstFormat != _dstFormat)
    {
        logger.i("Recreate sws context cause last format[%d] != current format[%d]",
                 dstFormat, _dstFormat);
        // Cleanup
        sws_freeContext(swsCtx);
        swsCtx = nullptr;
    }

    if (!swsCtx)
    {
        // Create SwsContext
        swsCtx = sws_getContext(
                srcFrame->width, srcFrame->height, (AVPixelFormat)srcFrame->format, // Source
                srcFrame->width, srcFrame->height, dstFormat,                // Destination
                SWS_BILINEAR, nullptr, nullptr, nullptr);
    }

    if (!swsCtx)
    {
        logger.i("Cannot create swsCtx");
        return false;
    }

    AVFrame *dstFrame = av_frame_alloc();
    if (!dstFrame)
    {
        logger.i("Cannot create AVFrame");
        return false;
    }

    // 关键字段手动拷贝过来
    dstFrame->format = dstFormat;
    dstFrame->width = srcFrame->width;
    dstFrame->height = srcFrame->height;
    dstFrame->pts = srcFrame->pts;
    dstFrame->pkt_dts = srcFrame->pkt_dts;
    dstFrame->time_base = srcFrame->time_base;
    dstFrame->duration = srcFrame->duration;

    if (av_frame_get_buffer(dstFrame, 32) < 0) {
        logger.i("Failed to allocate buffer for destination frame");
        sws_freeContext(swsCtx);
        swsCtx = nullptr;
        return false;
    }

    bool success = false;
    // Perform the conversion
    if (sws_scale(
            swsCtx,
            srcFrame->data, srcFrame->linesize, 0, srcFrame->height,
            dstFrame->data, dstFrame->linesize) < 0) {
        logger.i("sws_scale failed");
        av_frame_free(&dstFrame);
    } else {
        success =  true;
    }

    if (success)
    {
        *_dstFrame = dstFrame;
    }

    return success;
}
