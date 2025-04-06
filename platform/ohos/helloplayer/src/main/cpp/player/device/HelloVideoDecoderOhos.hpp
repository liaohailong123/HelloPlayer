//
// Created on 2025/3/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OHOS_HELLOAVDECODEROHOS_H
#define OHOS_HELLOAVDECODEROHOS_H

#include <mutex>
#include <queue>
#include <unordered_map>

#include "framework/player/device/HelloAVDecoderCore.hpp"

// 鸿蒙平台 视频解码
#include "multimedia/player_framework/native_avbuffer_info.h"
#include "multimedia/player_framework/native_avcodec_videodecoder.h"
// 官方文档：
// https://developer.huawei.com/consumer/cn/doc/harmonyos-guides-V14/avcodec-support-formats-V14#%E8%A7%86%E9%A2%91%E8%A7%A3%E7%A0%81

extern "C"
{
#include <libavcodec/bsf.h>
}

/**
 * create by liaohailong
 * 2025/3/15 16:47
 * desc: FFmpeg实现音视频解码器
 */
class HelloVideoDecoderOhos : public HelloAVDecoderCore
{
public:
    typedef struct
    {
        OH_AVCodec *codec;
        uint32_t index;
        OH_AVBuffer *buffer;
        void *userData;
    } OhosBufferData;

public:
    explicit HelloVideoDecoderOhos();
    ~HelloVideoDecoderOhos() override;
    
    
    bool needAnnexB() override;

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

    int sendPacket2(std::shared_ptr<IAVPacket> packet);

    /**
     * 释放解码器
     */
    virtual void release() override;

private:
    void putPacketDuration(int64_t pts, int64_t duration);
    bool getPacketDuration(int64_t pts, int64_t *duration);
    static void OnCodecError(OH_AVCodec *codec, int32_t errorCode, void *userData);
    static void OnCodecFormatChange(OH_AVCodec *codec, OH_AVFormat *format, void *userData);
    static void OnNeedInputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData);
    static void OnNewOutputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData);
    static bool readSample(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, std::shared_ptr<IAVPacket> packet,
                           void *userData);
    static bool writeSample(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, std::shared_ptr<IAVFrame> frame,
                            void *userData);


private:
    const char *name;
    OH_AVCodec *decoder;
    AVRational time_base;

    std::atomic<bool> running;
    int32_t colorFormat;


    // 输入待解码的数据包
    std::mutex bufferQueueMutex;
    std::condition_variable bufferQueueCond;
    std::queue<OhosBufferData> bufferQueue;

    std::mutex packetDurationMutex;
    std::unordered_map<int64_t, int64_t> packetDurationMap; // ptsUs to durationUs
    

    int32_t picWidth;
    int32_t picHeight;
    int32_t widthStride;
    int32_t heightStride;
};

#endif // OHOS_HELLOAVDECODEROHOS_H
