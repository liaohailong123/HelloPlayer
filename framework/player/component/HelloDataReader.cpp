//
// Created by 廖海龙 on 2025/1/8.
//

#include "HelloDataReader.hpp"

#include <utility>


HelloDataReader::HelloDataReader(std::shared_ptr<HelloAVSync> avSync) :
        HelloProcessor("HelloDataReader"), avSync(std::move(avSync)),
        url(), status(Idle), error(HelloPlayerError::None), connectTimeUs(0),
        prepared(false), mutex(), ifmt_ctx(nullptr), audio_index(-1),
        video_index(-1), ioTimeoutUs(HELLO_TIMEOUT_US)
{
    logger.i("HelloDataReader::HelloDataReader(%p)", this);
}


HelloDataReader::~HelloDataReader()
{
    reset();

    logger.i("HelloDataReader::HelloDataReader", this);
}


void HelloDataReader::setDataSource(std::string &_url)
{
    this->url = _url;
}


bool HelloDataReader::prepare(int64_t startOffsetPtsUs, const PlayConfig &playConfig)
{
    this->ioTimeoutUs = playConfig.ioTimeoutUs;
    if (this->url.empty())
    {
        logger.i("uri is empty");
        error = HelloPlayerError::NoDataSource;
        return false;
    }

    if (prepared)
    {
        logger.i("data processor is already prepared");
        return true;
    }

    if (ifmt_ctx)
    {
        logger.i("data processor's ifmt_ctx is already exist");
        return true;
    }

    int ret;
    const char *filepath = url.c_str();

    AVDictionary *options = nullptr;

    if (filepath && std::strncmp(filepath, "rtsp://", 7) == 0)
    {
        // 设置RTSP传输方式为TCP
        av_dict_set(&options, "rtsp_transport", "tcp", 0);
        logger.i("use -rtsp_transport tcp");
    }

    ifmt_ctx = avformat_alloc_context();

    // 快速打断IO，退出
    AVIOInterruptCB cb = {
            .callback = decodeInterruptCb,
            .opaque = this
    };
    ifmt_ctx->interrupt_callback = cb;

    // 打开文件IO
    status = Connecting;
    connectTimeUs = av_gettime_relative(); // 记录开始连接时间,在 decodeInterruptCb 中实现网络连接超时机制
    ret = avformat_open_input(&ifmt_ctx, filepath, nullptr, &options);
    av_dict_free(&options);
    if (ret != 0 || !ifmt_ctx)
    {
        status = Idle;
        FFUtil::av_print_error(logger, ret, "avformat_open_input error");
        // 在 decodeInterruptCb 中设置了 error ;
        return false;
    }
    status = Connected;

    // 读取一些轨道信息，获得 视频尺寸 + 音频基本信息
    if (avformat_find_stream_info(ifmt_ctx, nullptr) < 0)
    {
        FFUtil::av_print_error(logger, ret, "avformat_find_stream_info error");
        error = NoStreamInDataSource;
        return false;
    }

    // 找出最佳音频轨道并选中它
    ret = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (ret < 0)
    {
        FFUtil::av_print_error(logger, ret, "av_find_best_stream[AUDIO] error");
    } else
    {
        audio_index = ret;
    }

    // 找出最佳视频轨道并选中它
    ret = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (ret < 0)
    {
        FFUtil::av_print_error(logger, ret, "av_find_best_stream[VIDEO] error");
    } else
    {
        video_index = ret;
    }

    // 音视频轨道都没有？
    if (audio_index < 0 && video_index < 0)
    {
        error = NoAudioVideoStream;
        logger.i("Cannot found audio or video stream");
        return false;
    }
    int _audio_index = audio_index;
    int _video_index = video_index;
    logger.i("audio_index[%d] video_index[%d]", _audio_index, _video_index);


    // 判断是否需要处理起始偏移值
    if (startOffsetPtsUs)
    {
        prepared = streamSeek(startOffsetPtsUs);
    } else
    {
        prepared = true;
    }

    if (prepared)
    {
        status = Prepared;
        logger.i("prepare success");
    } else
    {
        status = Connected;
        logger.i("prepare failure");
    }

    error = HelloPlayerError::None;
    return status >= Prepared;
}

AVStream *HelloDataReader::getAudioStream()
{
    if (prepared && hasAudio())
    {
        return ifmt_ctx->streams[audio_index];
    }
    return nullptr;
}

AVStream *HelloDataReader::getVideoStream()
{
    if (prepared && hasVideo())
    {
        return ifmt_ctx->streams[video_index];
    }
    return nullptr;
}

bool HelloDataReader::getMediaInfo(Hello::MediaInfo *mediaInfo)
{
    if (!prepared)return false;
    if (!mediaInfo)return false;

    mediaInfo->url = url;
    mediaInfo->startOffsetUs = ifmt_ctx != nullptr ? ifmt_ctx->start_time : 0; // m3u8比较多
    mediaInfo->audioDurationUs = -1;
    mediaInfo->videoDurationUs = -1;
    mediaInfo->sampleRate = -1;
    mediaInfo->channelCount = -1;
    mediaInfo->sampleFmt = -1;
    mediaInfo->width = -1;
    mediaInfo->height = -1;
    mediaInfo->frameRate = {0, 1};

    AVStream *stream;

    stream = getAudioStream();
    if (stream)
    {
        mediaInfo->audioDurationUs = getDurationUs(stream->duration, stream->time_base);
        mediaInfo->sampleRate = stream->codecpar->sample_rate;
        mediaInfo->channelCount = stream->codecpar->ch_layout.nb_channels;
        mediaInfo->sampleFmt = stream->codecpar->format;
    } else
    {
        logger.i("getMediaInfo: Cannot found audio stream");
    }

    stream = getVideoStream();
    if (stream)
    {
        mediaInfo->videoDurationUs = getDurationUs(stream->duration, stream->time_base);
        mediaInfo->width = stream->codecpar->width;
        mediaInfo->height = stream->codecpar->height;
        mediaInfo->frameRate = stream->codecpar->framerate;
    } else
    {
        logger.i("getMediaInfo: Cannot found video stream");
    }

    // 判断是否为直播流
    if (ifmt_ctx->duration == AV_NOPTS_VALUE ||
        mediaInfo->audioDurationUs == AV_NOPTS_VALUE ||
        mediaInfo->videoDurationUs == AV_NOPTS_VALUE)
    {
        mediaInfo->isLiveStreaming = true;
        mediaInfo->audioDurationUs = -1;
        mediaInfo->videoDurationUs = -1;
    } else
    {
        mediaInfo->isLiveStreaming = false;
    }

    return true;
}

bool HelloDataReader::streamSeek(int64_t startOffsetPtsUs)
{

    if (ifmt_ctx)
    {
        int64_t adjustStartUs = ifmt_ctx->start_time + startOffsetPtsUs;
        logger.i("streamSeek[%d]ms start", adjustStartUs / 1000);

        // AVSEEK_FLAG_BACKWARD 查找时寻找靠近目标时间戳的最早的关键帧，即向前找I帧（关键帧）
        int ret = av_seek_frame(ifmt_ctx, -1, adjustStartUs, AVSEEK_FLAG_BACKWARD);
//        int ret = av_seek_frame(ifmt_ctx, -1, adjustStartUs, 0); // ffplay.c 这里是默认0
        if (ret < 0)
        {
            char msg[1024] = {0};
            sprintf(msg, "avformat_seek_file[%ld]ms error", static_cast<long>(adjustStartUs / 1000));
            FFUtil::av_print_error(logger, ret, msg);
            return false;
        }
        logger.i("streamSeek[%d]ms success", adjustStartUs / 1000);
        return true;
    }

    // 下面这种方式也可以
//    if (ifmt_ctx)
//    {
//        int64_t adjustStartUs = ifmt_ctx->start_time + startOffsetPtsUs;
//        logger.i("streamSeek[%d]ms start", adjustStartUs / 1000);
//
//        // AVSEEK_FLAG_BACKWARD 查找时寻找靠近目标时间戳的最早的关键帧，即向前找I帧（关键帧）
//        int ret = avformat_seek_file(ifmt_ctx, -1, INT64_MIN, adjustStartUs, INT64_MAX,
//                           AVSEEK_FLAG_BACKWARD);
//        if (ret < 0)
//        {
//            char msg[1024] = {0};
//            sprintf(msg, "avformat_seek_file[%ld]ms error", adjustStartUs / 1000);
//            FFUtil::av_print_error(logger, ret, msg);
//            return false;
//        }
//        logger.i("streamSeek[%d]ms success", adjustStartUs / 1000);
//        return true;
//    }

    logger.i("streamSeek[%ld]ms error cause ifmt_ctx is nullptr", startOffsetPtsUs / 1000);
    return false;
}

int64_t HelloDataReader::getDurationUs(int64_t durationPts, AVRational timeBase)
{
    if (AV_NOPTS_VALUE == durationPts)
    {
        return AV_NOPTS_VALUE;
    }

    int64_t durationUs = av_rescale_q(durationPts, timeBase, AV_TIME_BASE_Q);

    if (ifmt_ctx && durationUs < 0)
    {
        durationUs = ifmt_ctx->duration;
    }
    return durationUs;
}

void HelloDataReader::sendFlushBufferPacket(IAVMediaType type)
{
    HelloClock &masterClock = avSync->getMasterClock();
    std::shared_ptr<IAVPacket> output = std::make_shared<IAVPacket>();
    output->type = type;
    output->serial = masterClock.serial;
    output->flushBuffers = true; // 发送flush包
    sendCallback(output);
}

void HelloDataReader::sendEofBufferPacket(IAVMediaType type)
{
    HelloClock &masterClock = avSync->getMasterClock();
    std::shared_ptr<IAVPacket> output = std::make_shared<IAVPacket>();
    output->type = type;
    output->serial = masterClock.serial;
    output->eof = true; // 发送eof包
    sendCallback(output);
}


bool HelloDataReader::onProcess(std::shared_ptr<InputDataCtx> inputData)
{
    logger.i("HelloDataReader::onProcess");
    if (!prepared)return false;

    // 修改资源操作，需挂锁
    std::unique_lock<std::mutex> locker(mutex);

    HelloClock &masterClock = avSync->getMasterClock();
    std::shared_ptr<PlayRange> &range = inputData->data;
    logger.i("onProcess is start range[%d,%d]ms",
             range->startUs / 1000, range->endUs / 1000);

    if (range->seekOnce)
    {
        // 发送flush包，通知到下一个消费者（音视频解码器）做相应处理
        sendFlushBufferPacket(IAVMediaType::AUDIO);
        sendFlushBufferPacket(IAVMediaType::VIDEO);
        logger.i("send flush packet before stream seek");

        // 偏移音视频轨道的数据读取位置（靠前的I帧），接下来数据加载生效
        if (!streamSeek(range->startUs))
        {
            return true;
        }
        range->seekOnce = false;
    }

    AVPacket *pkt = av_packet_alloc();
    int ret;
    bool audioInRange = hasAudio();
    bool videoInRange = hasVideo();

    while (prepared)
    {
        // 内部会创建 AVBufferRef，但是不会 av_packet_unref
        // demux.c -> int ff_read_packet(AVFormatContext *s, AVPacket *pkt)
        ret = av_read_frame(ifmt_ctx, pkt);

        // 序列号不一致，停止解复用
        int64_t masterSerial = masterClock.serial;
        if (range->serial != masterSerial)
        {
            // 发送flush包，通知到下一个消费者（音视频解码器）做相应处理
            sendFlushBufferPacket(IAVMediaType::AUDIO);
            sendFlushBufferPacket(IAVMediaType::VIDEO);
            logger.i("send flush packet while decoding",
                     range->serial, masterSerial);
            break;
        }
        if (ret >= 0 && prepared)
        {
            std::shared_ptr<IAVPacket> output = std::make_shared<IAVPacket>();
            // 设置序列号
            output->serial = range->serial;
//            logger.i("pkt->stream_index[%d]", pkt->stream_index);

            // 确定编码数据类型，属于哪个轨道
            if (audio_index >= 0 && pkt->stream_index == audio_index)
            {
                output->type = AUDIO; // 音轨

                // 判断音频是否超出播放范围
                AVRational time_base = ifmt_ctx->streams[audio_index]->time_base;
                int64_t ptsUs = av_rescale_q(pkt->pts, time_base, AV_TIME_BASE_Q);
                int64_t endUs = range->getEndPts();
                audioInRange = ptsUs < endUs;
                pkt->time_base = time_base; // 必须手动添加
//                logger.i("pts[%d]ms end[%d]ms audioInRange[%d]",
//                         ptsUs / 1000, endUs / 1000, audioInRange);
            } else if (video_index >= 0 && pkt->stream_index == video_index)
            {
                output->type = VIDEO; // 视轨

                // 判断视频是否超出播放范围
                AVRational time_base = ifmt_ctx->streams[video_index]->time_base;
                int64_t ptsUs = av_rescale_q(pkt->pts, time_base, AV_TIME_BASE_Q);
                int64_t endUs = range->getEndPts();
                videoInRange = ptsUs < endUs;
                pkt->time_base = time_base; // 必须手动添加
//                logger.i("pts[%d]ms end[%d]ms videoInRange[%d]",
//                         ptsUs / 1000, endUs / 1000, videoInRange);
            } else
            {
                output->type = UNKNOWN;
            }

            // 将packet的buffer内容迁移给外部
            av_packet_move_ref(output->packet, pkt);

//            if (output->packet->flags | AV_PKT_FLAG_KEY)
//            {
//                logger.i("output->packet[%d] pts[%d]ms Key Frame",
//                         output->type, output->getPtsUs() / 1000);
//            }
            // 回调外部观察者
            sendCallback(output);

            // 判断时间是否超出 本次播放范围
            if (!audioInRange && !videoInRange)
            {
                // 超出播放范围，终止取编码数据
                logger.i("out of play range");
                break;
            }

        } else if (ret == AVERROR_EOF)
        {
            sendEofBufferPacket(IAVMediaType::AUDIO);
            sendEofBufferPacket(IAVMediaType::VIDEO);
            logger.i("read EOF");
            break;
        } else
        {
            FFUtil::av_print_error(logger, ret, "av_read_frame error");
        }
    }

    av_packet_free(&pkt);
    logger.i("onProcess is over");
    return true;
}

void HelloDataReader::onReset()
{
    status = Idle;
    prepared = false;

    logger.i("HelloDataReader::onReset() start");
    // 修改资源操作，需挂锁
    std::unique_lock<std::mutex> locker(mutex);


    audio_index = -1;
    video_index = -1;
    url = "";

    if (ifmt_ctx)
    {
        avformat_close_input(&ifmt_ctx);
        ifmt_ctx = nullptr;
    }

    logger.i("HelloDataReader::onReset() end");
}

int HelloDataReader::decodeInterruptCb(void *userdata)
{
    auto native = reinterpret_cast<HelloDataReader *>(userdata);
    Logger &logger = native->logger;
    long ioTimeoutUs = native->ioTimeoutUs;

    // 连接状态判断是否网络连接超时
    if (Idle < native->status && native->status <= Connecting)
    {
        int64_t currTimeUs = av_gettime_relative();
        int64_t diffUs = currTimeUs - native->connectTimeUs;
        logger.i("HelloDataReader::decodeInterruptCb currTime[%d]ms - connectTime[%d]ms",
                 currTimeUs / 1000, native->connectTimeUs / 1000);
        if (diffUs > ioTimeoutUs)
        {
            logger.i("HelloDataReader::decodeInterruptCb timeout[%d]ms",
                     ioTimeoutUs / 1000);
            native->error = HelloPlayerError::NetworkTimeout;
            return 1;
        }

        return 0;
    } else if (Connecting < native->status && native->status <= Prepared)
    {
        return 0;
    }

    logger.i("HelloDataReader::decodeInterruptCb native->prepared[false]");
    return 1;
}