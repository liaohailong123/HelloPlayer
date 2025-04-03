//
// Created by liaohailong on 2024/12/19.
//

#include "IAVPacket.hpp"

IAVPacket::IAVPacket(const char *tag) : logger(tag),
                                        type(IAVMediaType::UNKNOWN), flushBuffers(false),
                                        serial(0), eof(false)
{
//    logger.i("IAVPacket::IAVPacket(%p)", this);

    packet = av_packet_alloc();
    if (!packet)
    {
        logger.i("Couldn't alloc packet");
    }
}

IAVPacket::~IAVPacket()
{
    if (packet)
    {
        av_packet_free(&packet);
        packet = nullptr;
    }
//    logger.i("IAVPacket::~IAVPacket(%p)", this);
}

int64_t IAVPacket::getPtsUs()
{
    if (packet)
    {
        // pts to us
        int64_t pts_us = av_rescale_q(packet->pts, packet->time_base, (AVRational) {1, 1000000});
        return pts_us;
    }
    return 0;
}

int64_t IAVPacket::getDurationUs()
{
    if (packet)
    {
        // pts to us
        int64_t d_us = av_rescale_q(packet->duration, packet->time_base, (AVRational) {1, 1000000});
        return d_us;
    }
    return 0;
}

int64_t IAVPacket::getDtsUs()
{
    if (packet)
    {
        // dts to us
        int64_t d_us = av_rescale_q(packet->dts, packet->time_base, (AVRational) {1, 1000000});
        return d_us;
    }
    return 0;
}

const char *IAVPacket::getMediaTypeDesc() const
{
    switch (type)
    {
        case IAVMediaType::AUDIO:
            return "audio";
        case IAVMediaType::VIDEO:
            return "video";
        default:
            break;
    }
    return "Unknown";
}