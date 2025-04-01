//
// Created by 廖海龙 on 2025/1/8.
//

#ifndef HELLOPLAYER_HELLODATAREADER_HPP
#define HELLOPLAYER_HELLODATAREADER_HPP

#include "HelloProcessor.hpp"
#include <string>
#include "../entity/IAVPacket.hpp"
#include "../entity/PlayRange.hpp"
#include "../entity/MediaInfo.hpp"
#include "../entity/PlayConfig.hpp"
#include "../HelloAVSync.hpp"
#include "../HelloPlayerError.hpp"

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/imgutils.h>
#include <libavutil/timestamp.h>
#include <libavutil/time.h>
}


// 默认网络连接超时15秒
#define HELLO_TIMEOUT_US (15000000)

/**
 * Author: liaohailong
 * Date: 2025/1/8
 * Time: 11:21
 * Description: 读取数据模块
 **/
class HelloDataReader : public HelloProcessor<PlayRange, IAVPacket>
{
private:
    enum Status
    {
        Idle,
        Connecting,
        Connected,
        Prepared
    };
public:
    explicit HelloDataReader(std::shared_ptr<HelloAVSync> avSync);

    ~HelloDataReader() override;

    void setDataSource(std::string &url);

    bool prepare(int64_t startOffsetPtsUs, const PlayConfig &playConfig);

    inline bool isPrepared() const noexcept
    {
        return prepared;
    }

    inline bool hasAudio() const noexcept { return audio_index >= 0; }

    inline bool hasVideo() const noexcept { return video_index >= 0; }

    AVStream *getAudioStream();

    AVStream *getVideoStream();

    /**
     * 获取多媒体信息
     * @param mediaInfo 接收多媒体信息的指针
     * @return true表示成功获取
     */
    bool getMediaInfo(Hello::MediaInfo *mediaInfo);

    /**
     * @return 获取错误码
     */
    inline HelloPlayerError getError() const
    {
        return this->error;
    }

    /**
     * 偏移文件读取位置
     * @param startOffsetPtsUs 偏移时间戳，单位：us
     * @return true表示 seek 成功
     */
    bool streamSeek(int64_t startOffsetPtsUs);

protected:
    bool onProcess(std::shared_ptr<InputDataCtx> inputData) override;
    void onReset() override;
    static int decodeInterruptCb(void* userdata);

private:
    int64_t getDurationUs(int64_t durationPts, AVRational timeBase);
    void sendFlushBufferPacket(IAVMediaType type);
    void sendEofBufferPacket(IAVMediaType type);


private:
    /**
     * 同步器
     */
    std::shared_ptr<HelloAVSync> avSync;
    /**
     * 资源url
     */
    std::string url;

    /**
     * 当前的状态
     */
    std::atomic<Status> status;

    /**
     * 当前错误状态
     */
    std::atomic<HelloPlayerError> error;

    /**
     * 开始连接时,记录一个时间戳
     */
    std::atomic<int64_t> connectTimeUs;

    /**
     * 是否已经准备好解复用
     */
    std::atomic<bool> prepared;

    /**
     * 外部线程与内部handler操作相应需要同步
     */
    std::mutex mutex;

    /**
     * 解复用
     */
    AVFormatContext *ifmt_ctx;

    /**
     * 音频轨道下标
     */
    std::atomic<int> audio_index;
    /**
     * 视频轨道下标
     */
    std::atomic<int> video_index;
    
    /**
     * IO连接超时(网络IO,文件IO)
     */
    long ioTimeoutUs;

};


#endif //HELLOPLAYER_HELLODATAREADER_HPP
