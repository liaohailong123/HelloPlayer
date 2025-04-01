//
// Created by liaohailong on 2024/12/20.
//

#ifndef HELLOPLAYER_IAVFRAME_HPP
#define HELLOPLAYER_IAVFRAME_HPP

extern "C" {
#include <libavutil/frame.h>
};


#include "../../log/Logger.hpp"
#include "../../util/FFUtil.hpp"
#include "IAVTime.hpp"
#include "IAVMediaType.hpp"

/**
 * create by liaohailong
 * 2024/12/20 20:11
 * desc: 解码好的数据
 */
class IAVFrame : public IAVTime
{
public:
    explicit IAVFrame(const char *tag = "IAVFrame");

    ~IAVFrame() override;

    int64_t getPtsUs() override;

    int64_t getDurationUs() override;

    /**
     * @param src 转移src数据自身frame上
     */
    void move(AVFrame* src);

    /**
     * @return 获取声道数量
     */
    int getChannelCount() const;

    /**
     * @return 单个样本，占多少字节
     */
    int getBytesPerSample() const;

    /**
     * 根据样本数量 读取到缓存buffer
     * @param audioData 缓存buffer
     * @param offset 缓存buffer的字节偏移量，先偏移再填充数据
     * @param nb_samples 样本数量
     * @return 实际写入了多少个样本
     */
    int readAudioSamples(void *audioData, int64_t offset, int nb_samples);


public:
    Logger logger;

    /**
     * 解码数据实现对象
     */
    AVFrame *frame;

    /**
     * 媒体数据类型
     */
    IAVMediaType type;

    /**
     * 序列号，用来区分包是否过期，或则无效，默认从0开始
     */
    int64_t serial;

    /**
     * End of file
     */
    bool eof;


private:
    /**
     * 音频数据，已经被读取了多少
     */
    int readSamples;

};


#endif //HELLOPLAYER_IAVFRAME_HPP
