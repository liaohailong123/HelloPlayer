//
// Created by liaohailong on 2024/12/19.
//

#ifndef HELLOPLAYER_IAVPACKET_HPP
#define HELLOPLAYER_IAVPACKET_HPP

#include <cstdint>

extern "C" {
#include <libavcodec/packet.h>
#include <libavutil/mathematics.h>
}

#include "../../log/Logger.hpp"
#include "IAVTime.hpp"
#include "IAVMediaType.hpp"

/**
 * create by liaohailong
 * 2024/12/19 21:45
 * desc: 编码数据，统一规范
 */
class IAVPacket : public IAVTime
{
public:
    explicit IAVPacket(const char *tag = "IAVPacket");

    ~IAVPacket() override;

    int64_t getPtsUs() override;

    int64_t getDurationUs() override;

    int64_t getDtsUs();

public:
    Logger logger;

    /**
     * 数据类型，音频数据/视频数据
     */
    IAVMediaType type;

    /**
     * 不同编码数据的实现对象
     */
    AVPacket *packet;

    /**
     * flushBuffers为true表示为空包，为控制解码器 avcodec_flush_buffers()
     */
    bool flushBuffers;

    /**
     * End of file
     */
    bool eof;

    /**
     * 序列号，用来区分包是否过期，或则无效，默认从0开始
     */
    int64_t serial;
};


#endif //HELLOPLAYER_IAVPACKET_HPP
