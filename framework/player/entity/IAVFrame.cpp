//
// Created by liaohailong on 2024/12/20.
//

#include "IAVFrame.hpp"


IAVFrame::IAVFrame(const char *tag) : logger(tag), frame(nullptr),
                                      type(UNKNOWN), serial(0), eof(false), readSamples(0)
{
//    logger.i("IAVFrame::IAVFrame(%p)", this);

    frame = av_frame_alloc();
    if (!frame)
    {
        logger.i("Couldn't alloc frame");
    }
}

IAVFrame::~IAVFrame()
{
    if (frame)
    {
        av_frame_free(&frame);
        frame = nullptr;
    }
//    logger.i("IAVFramree::~IAVFrame(%p)", this);
}

int64_t IAVFrame::getPtsUs()
{
    if (frame)
    {
        // pts to us
        int64_t pts_us = av_rescale_q(frame->pts, frame->time_base, (AVRational) {1, 1000000});
        return pts_us;
    }
    return 0;
}

int64_t IAVFrame::getDurationUs()
{
    if (frame)
    {
        // pts to us
        int64_t d_us = av_rescale_q(frame->duration, frame->time_base, (AVRational) {1, 1000000});
        return d_us;
    }
    return 0;
}

void IAVFrame::move(AVFrame *src)
{
    // 已经存在数据，销毁重新创建
    if (frame->width > 0 || frame->height > 0 || frame->nb_samples > 0)
    {
        av_frame_free(&frame);
        frame = av_frame_alloc();
    }
    av_frame_move_ref(frame, src);
    readSamples = 0;
}

int IAVFrame::getChannelCount() const
{
    if (frame)
    {
        return frame->ch_layout.nb_channels;
    }
    return 0;
}

/**
 * @return 单个样本，占多少字节
 */
int IAVFrame::getBytesPerSample() const
{
    return av_get_bytes_per_sample((AVSampleFormat) frame->format);
}

int IAVFrame::readAudioSamples(void *audioData, int64_t offset, int nb_samples)
{
    int count = 0;
    if (frame)
    {
        int remainingSamples = frame->nb_samples - readSamples;
        if (remainingSamples <= 0)
        {
            // 无数据可读了
            return count;
        }
        count = FFMIN(nb_samples, remainingSamples);
        int nb_channels = getChannelCount();
        int bytes_per_sample = av_get_bytes_per_sample((AVSampleFormat) frame->format);
        uint8_t **src_data = frame->extended_data;
        uint8_t *dst_pos = reinterpret_cast<uint8_t *>(audioData);
        dst_pos += offset;
        AVSampleFormat samplerFormat = (AVSampleFormat) frame->format;
        bool planner = FFUtil::is_planner_fmt(samplerFormat);

        if (planner)
        {
            // planner平铺格式 转换 到packet交错格式
            for (int i = readSamples; i < readSamples + count; ++i)
            {
                for (int ch = 0; ch < nb_channels; ch++)
                {
                    memcpy(dst_pos, src_data[ch] + i * bytes_per_sample, bytes_per_sample);
                    dst_pos += bytes_per_sample;
                }
            }
        } else
        {
            // 交错模式数据直接拷贝
            int64_t bytesPerFrame = bytes_per_sample * nb_channels;
            int64_t startOffset = readSamples * bytesPerFrame;
            int64_t readBytes = count * bytesPerFrame;
            memcpy(dst_pos, src_data[0] + startOffset, readBytes);
        }

        readSamples += count;
    }

    return count;
}
