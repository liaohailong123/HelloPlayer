//
// Created by liaohailong on 2024/12/19.
//

#include "HelloPlayer.hpp"

HelloPlayer::HelloPlayer(void *userdata) : logger("HelloPlayerCpp"), callback(nullptr),
                                           playConfig(), userSeekUs(0), mediaInfo(),
                                           dataReader(nullptr), audioDecoder(nullptr),
                                           videoDecoder(nullptr), audioPlayer(nullptr),
                                           videoPlayer(nullptr), currentPlayRange(),
                                           decodeBufferRange(), pendingSurface(nullptr),
                                           playerThread(), playerHandler(nullptr),
                                           uri(), state(PlayState::Idle),
                                           avSync(new HelloAVSync()),
                                           seekAction(nullptr)
{
    logger.i("HelloPlayer::HelloPlayer(%p)", this);
    // 开启子线程：播放器主流程
    playerThread = new NHandlerThread("HelloPlayerThread");
    playerThread->startSync();
    playerHandler = playerThread->getHandler();
    logger.i("playerHandler(%p) already be done", playerHandler);

    // 获取平台侧回调对象
    callback = HelloPlayerCallback::createHelloPlayerCallback(userdata);

    playerHandler->post(&HelloPlayer::registerAll, this);
}

HelloPlayer::~HelloPlayer()
{

    if (playerThread)
    {
        delete playerThread;
        playerThread = nullptr;
        playerHandler = nullptr;
    }


    logger.i("HelloPlayer::~HelloPlayer(%p)", this);
}

void HelloPlayer::setConfig(PlayConfig config)
{
    playConfig = config; // 值传递
}

PlayConfig HelloPlayer::getConfig()
{
    return playConfig; // 值传递
}

void HelloPlayer::setDataSource(const char *_uri)
{
    logger.i("HelloPlayer(%p)::setDataSource:\n%s\n", this, _uri);
    this->uri = _uri;
}

void HelloPlayer::addSurface(void *s)
{
    if (videoPlayer)
    {
        logger.i("videoPlayer is exist and add surface[%p]", s);
        videoPlayer->addSurface(s);
    } else
    {
        logger.i("videoPlayer is not exist and save surface[%p]", s);
        pendingSurface = s;
    }
}

void HelloPlayer::removeSurface(void *s)
{
    if (videoPlayer)
    {
        videoPlayer->removeSurface(s);
    }
}

void HelloPlayer::prepare(int64_t ptsUs)
{
    logger.i("HelloPlayer(%p)::prepare pts[%d]ms", this, ptsUs / 1000);

    // 设置起始偏移值
    userSeekUs = ptsUs;

    // 初次初始化
    if (playerHandler)
    {
        playerHandler->post(&HelloPlayer::doPrepare, this);
    } else
    {
        // 准备失败
        updateState(Idle);
    }
}

void HelloPlayer::start()
{
    logger.i("HelloPlayer(%p)::start", this);
    if (isPlaying())
    {
        logger.i("player is running");
        return;
    }

    if (playerHandler)
    {
        playerHandler->post(&HelloPlayer::doStart, this);
    }
}

bool HelloPlayer::isPrepared()
{
    return state >= Prepared;
}

bool HelloPlayer::isPlaying()
{
    return state == Playing;
}

bool HelloPlayer::isSeeking()
{
    return state == Seeking;
}

void HelloPlayer::seekTo(int64_t ptsUs, bool _autoPlay)
{
    logger.i("HelloPlayer(%p)::seekTo(%ld,%d)", this, ptsUs, _autoPlay);

    if (mediaInfo.isLiveStreaming)
    {
        logger.i("This is a live streaming, seeking is not allowed");
        return;
    }

    if (playerHandler)
    {
        seekAction = std::make_shared<SeekAction>();
        seekAction->seekToUs = ptsUs;
        seekAction->autoPlay = _autoPlay;
        playerHandler->post(&HelloPlayer::doSeek, this);
    }
}

void HelloPlayer::pause()
{
    if (playerHandler && isPlaying())
    {
        if (mediaInfo.isLiveStreaming)
        {
            logger.i("This is a live streaming, pause is not allowed");
            return;
        }
        playerHandler->post(HelloPlayer::doPause, this);
    }
}

/**
 * 调整音量大小
 * @param volume 0.0=静音 1.0=原始音量 大于1.0=扩大音量
 */
void HelloPlayer::setVolume(double volume)
{
    playConfig.volume = volume;
    if (isPrepared() && audioPlayer)
    {
        audioPlayer->setVolume(volume);
    }
}


/**
 * 调整播放速率
 * @param speed [0.5,100.0]
 */
void HelloPlayer::setSpeed(double speed)
{
    if (speed < 0.5)speed = 0.5; // 这个是av_filter的极限慢速

    if (mediaInfo.isLiveStreaming)
    {
        logger.i("This is a live streaming, setSpeed is not allowed");
        return;
    }

    playConfig.speed = speed;
    if (isPrepared() && audioPlayer)
    {
        avSync->getMasterClock().speed = speed;
        audioPlayer->setSpeed(speed);
    }
}


void HelloPlayer::reset()
{
    if (playerHandler)
    {
        playerHandler->post(HelloPlayer::onPlayerReset, this);
    }
}


void HelloPlayer::registerAll(void *userData)
{
    auto native = reinterpret_cast<HelloPlayer *>(userData);
    PlayConfig *playerConfig = &native->playConfig;

    // 初始化所有组件
    native->dataReader = std::make_shared<HelloDataReader>(native->avSync);
    native->audioDecoder = std::make_shared<HelloAVDecoder>("HelloAudioDecoder", native->avSync);
    native->videoDecoder = std::make_shared<HelloAVDecoder>("HelloVideoDecoder", native->avSync);
    native->audioPlayer = std::make_shared<HelloAudioPlayer>(native->avSync);
    native->videoPlayer = std::make_shared<HelloVideoPlayer>(native->avSync);

    // 消费者注册 数据回调（解复用拿出来的编码数据包）
    native->dataReader->setOutputCallback(onPacketRead, native);
    native->dataReader->start();

    // 消费者注册 数据回调（音频解码数据包）
    native->audioDecoder->setOutputCallback(onAudioDecoded, native);
    std::shared_ptr<HelloAVDecoder::OnDecodeCallbackCtx> audioCtx(
            new HelloAVDecoder::OnDecodeCallbackCtx);
    audioCtx->callback = HelloPlayer::OnBeforeAudioDecode;
    audioCtx->userdata = native;
    native->audioDecoder->setOnDecodeCallbackCtx(audioCtx); // 送解码前的拦截器
    native->audioDecoder->start();
    // 消费者注册 数据回调（视频解码数据包）
    native->videoDecoder->setOutputCallback(onVideoDecoded, native);
    std::shared_ptr<HelloAVDecoder::OnDecodeCallbackCtx> videoCtx(
            new HelloAVDecoder::OnDecodeCallbackCtx);
    videoCtx->callback = HelloPlayer::OnBeforeVideoDecode;
    videoCtx->userdata = native;
    native->videoDecoder->setOnDecodeCallbackCtx(videoCtx); // 送解码前的拦截器
    native->videoDecoder->start();

    // 消费者注册 数据回调（音频播放）
    native->audioPlayer->setOutputCallback(onAudioRendered, native);
    native->audioPlayer->setSpeed(playerConfig->speed); // 播放速率
    native->audioPlayer->setVolume(playerConfig->volume); // 音量大小
    native->audioPlayer->start();

    // 消费者注册 数据回调（视频播放）
    native->videoPlayer->setOutputCallback(onVideoRendered, native);
    native->videoPlayer->start();


    native->logger.i("HelloPlayer::registerAll");
}

void HelloPlayer::doPrepare(void *userData)
{
    auto native = reinterpret_cast<HelloPlayer *>(userData);
    Logger &logger = native->logger;
    PlayState state = native->state;

    logger.i("HelloPlayer(%p)::doPrepare", native);

    if (state >= Preparing)
    {
        logger.i("hello player is preparing, please wait a second");
        return;
    }

    // 进入准备状态
    native->updateState(Preparing);

    if (native->uri.empty())
    {
        logger.i("Uri is empty?");
        return;
    }

    bool opened = native->openSource();

    if (opened)
    {
        // 开启5大组件
        native->dataReader->start();
        native->audioDecoder->start();
        native->videoDecoder->start();
        native->audioPlayer->start();
        native->videoPlayer->start();

        // 是否自动起播
        if (native->playConfig.autoPlay)
        {
            native->start();
        }
    }
}

bool HelloPlayer::openSource()
{

    int64_t startOffsetUs = userSeekUs;

    logger.i("HelloPlayer::openSource");

    if (dataReader)
    {
        if (dataReader->isPrepared())
        {
            logger.i("open source is already prepared");
            return true;
        }
        dataReader->setDataSource(uri);
        dataReader->prepare(startOffsetUs, playConfig);

        logger.i("dataReader->prepare[%d] end", dataReader->isPrepared());
        // 获取多媒体信息
        if (dataReader->isPrepared())
        {
            // 获取媒体信息
            dataReader->getMediaInfo(&mediaInfo);

            logger.i("dataReader->getAudioStream() start");
            // 初始化解码器
            AVStream *audioStream = dataReader->getAudioStream();
            if (audioStream)
            {
                logger.i("audioDecoder->prepare start");
                audioDecoder->prepare(audioStream, playConfig);
                logger.i("audioDecoder->prepare end");

                // 初始化音频播放器
                std::shared_ptr<AudioProperties> p = audioDecoder->getAudioProperties();
                logger.i("audioPlayer->prepare start");
                audioPlayer->prepare(p, &playConfig);
                logger.i("audioPlayer->prepare start");
            }

            logger.i("dataReader->getVideoStream() start");
            AVStream *videoStream = dataReader->getVideoStream();
            if (videoStream)
            {
                logger.i("videoDecoder->prepare start");
                videoDecoder->prepare(videoStream, playConfig);
                logger.i("videoDecoder->prepare end");

                // 初始化视频播放器
                std::shared_ptr<VideoProperties> p = videoDecoder->getVideoProperties();
                videoPlayer->prepare(p, playConfig);
                // 尝试添加输出surface，用户可能设置的早，存起来了
                if (pendingSurface)
                {
                    logger.i("is time to add pending surface");
                    videoPlayer->addSurface(pendingSurface);
                    pendingSurface = nullptr;
                }
            }

            // 音视频同步，设置主时钟，按照优先级：先音频，后视频
            if (audioDecoder->isPrepared())
            {
                avSync->setMastClockType(IAVMediaType::AUDIO);
                logger.i("master clock is audio");
            } else if (videoDecoder->isPrepared())
            {
                avSync->setMastClockType(IAVMediaType::VIDEO);
                logger.i("master clock is video");
            } else
            {
                // 音视频轨道一个都没有准备好？
                logger.i("audio and video is not prepared?");
            }

            // 设置主时钟相关配置
            HelloClock &masterClock = avSync->getMasterClock();
            masterClock.setCallback(HelloPlayer::onMasterClockUpdate, this);
            masterClock.speed = playConfig.speed;
            mediaInfo.masterClockType = avSync->getMastClockType();

            // 打印一下媒体信息
            logger.i(mediaInfo.toString().c_str());

            // 回调
            callback->onPrepared(mediaInfo);
        }
    }

    bool openSuccess;
    // 更新状态
    if (dataReader && dataReader->isPrepared())
    {
        openSuccess = true;
        updateState(Prepared);
        logger.i("open source success");
    } else
    {
        openSuccess = false;
        // 更新状态
        updateState(Idle);

        // 回调错误信息
        HelloPlayerError error = dataReader->getError();
        const char *errorMsg = hello_get_player_error_msg(error);
        callback->onErrorInfo(error, errorMsg);

        logger.i("open source failure");
    }

    return openSuccess;
}

void HelloPlayer::doStart(void *userData)
{
    auto native = reinterpret_cast<HelloPlayer *>(userData);
    HelloClock &clock = native->avSync->getMasterClock();

    native->logger.i("HelloPlayer::doStart");

    // 是否从暂停中恢复
    if (clock.isPaused())
    {
        clock.setPaused(false);
    } else
    {
        // 起播
        native->loadNextPlayRange();
    }

    native->updateState(Playing);
}

void HelloPlayer::doPause(void *userData)
{
    auto native = reinterpret_cast<HelloPlayer *>(userData);
    HelloClock &clock = native->avSync->getMasterClock();

    native->updateState(Paused);
    // 设置主时钟，控制是否暂停
    clock.setPaused(true);

    native->logger.i("player is paused");
}

void HelloPlayer::doSeek(void *userData)
{
    auto native = reinterpret_cast<HelloPlayer *>(userData);

    PlayRange *bufferRange = &native->decodeBufferRange;
    PlayRange *currentRange = &native->currentPlayRange;
    std::shared_ptr<HelloAVSync> &avSync = native->avSync;
    std::shared_ptr<SeekAction> &seekAction = native->seekAction;

    std::shared_ptr<HelloDataReader> &dataReader = native->dataReader;
    std::shared_ptr<HelloAVDecoder> &audioDecoder = native->audioDecoder;
    std::shared_ptr<HelloAudioPlayer> &audioPlayer = native->audioPlayer;
    std::shared_ptr<HelloVideoPlayer> &videoPlayer = native->videoPlayer;

    if (seekAction)
    {
        // 增加序列号之后，数据读取 - 音视频解码 - 音视频渲染 各模块都会有相应的处理
        int64_t nextSerial = avSync->getMasterClockSerial() + 1;
        // 重设时钟
        avSync->reset(nextSerial);

        // 开始新的偏移
        int64_t seekToUs = seekAction->seekToUs;
        currentRange->seekToPtsUs(seekToUs);

        // 可能只seek不播放
        if (seekAction->autoPlay)
        {
            // 设置主时钟，控制是否暂停
            avSync->setPaused(false);
        } else
        {
            // 设置主时钟，控制是否暂停
            avSync->setPaused(true);
        }
        native->loadNextPlayRange(true);

        // 进入seek状态，等待 主时钟 回调时，改变状态
        native->updateState(Seeking);
    }

    // 一次性消费掉
    seekAction = nullptr;
}


void HelloPlayer::onSeekEnd(void *userData)
{
    auto native = reinterpret_cast<HelloPlayer *>(userData);

    // seek end 的判断，回调上层
    if (native->isSeeking())
    {
        native->updateState(SeekEnd);
    }
}

bool HelloPlayer::updateState(PlayState next)
{
    // 初始化 -> 准备中
    if (state == Idle && next != Preparing)
    {
        logger.i("only Preparing can be set when current is Idle");
        return false;
    }

    // 准备好 -> 其他操作状态
    if (next > Prepared && state < Prepared)
    {
        logger.i("source not prepared, please wait a second");
        return false;
    }

    PlayState oldState = state;
    state = next;

    // 回调状态给上层
    callback->onPlayStateChanged(oldState, state);

    return true;
}

bool HelloPlayer::loadNextPlayRange(bool seekOnce)
{
    if (currentPlayRange.eof)
    {
//        logger.i("Cannot load next play range cause end of file");
        return false;
    }
    if (dataReader && dataReader->isPrepared())
    {
        int64_t oldStartPtsUs = currentPlayRange.startUs;
        int64_t oldEndPtsUs = currentPlayRange.endUs;
        int64_t serial = avSync->getMasterClock().serial;
        currentPlayRange.serial = serial; // 配置本次加载的序列号

        // 先计算出下一段播放的范围
        std::shared_ptr<PlayRange> range = getNextPlayRange();
        // 同步到当前播放范围
        currentPlayRange.copy(*range);
        // seek操作单独设置值，一次性消费，不要影响 currentPlayRange
        range->seekOnce = seekOnce;
        // 驱动数据加载
        dataReader->pushInputData(range);

        logger.i("load next play range seekOnce[%d] [%d,%d]ms -> [%d,%d]ms masterClock.serial[%d]",
                 seekOnce,
                 oldStartPtsUs / 1000, oldEndPtsUs / 1000,
                 range->startUs / 1000, range->endUs / 1000,
                 serial);
        return true;
    }

    return false;
}

std::shared_ptr<PlayRange> HelloPlayer::getNextPlayRange()
{
    std::shared_ptr<PlayRange> currRange = std::make_shared<PlayRange>();
    currRange->copy(currentPlayRange);

    std::shared_ptr<PlayRange> nextRange = std::make_shared<PlayRange>();

    // 取出外部设置的 缓存buffer时长
    int64_t bufferDurationUs = playConfig.bufferDurationUs;

    // 初次加载
    if (currRange->startUs < 0)
    {
        // 第一次加载，起播时间为多媒体文件startOffsetTime时间+用户指定偏移时间
        currRange->startUs = mediaInfo.startOffsetUs + userSeekUs;
    } else
    {
        currRange->startUs = currRange->endUs;
    }
    currRange->endUs = currRange->startUs + bufferDurationUs;

    // 控制边界
    bool useAudio = avSync->getMastClockType() == IAVMediaType::AUDIO;
    int64_t durationUs = useAudio ? mediaInfo.audioDurationUs : mediaInfo.videoDurationUs;
    int64_t maxEndUs = mediaInfo.startOffsetUs + durationUs;
    if (durationUs < 0)
    {
        // 直播流
        maxEndUs = INT64_MAX;
    }
    logger.i("getNextPlayRange useAudio[%d] duration[%d]ms", useAudio, durationUs / 1000);
    if (currRange->endUs > maxEndUs)
    {
        currRange->endUs = maxEndUs;
        currRange->eof = true; // End of file
    }

    // 把上面计算好的数据同步过来
    nextRange->copy(*currRange);

    return nextRange;
}


void HelloPlayer::updateBufferRange(const std::shared_ptr<IAVFrame> &frame)
{
    // 数据读取结束是依靠这个eof标记位，这里就不做更新缓冲区的操作了
    if (frame->eof) return;

    // 更新缓冲区范围
    decodeBufferRange.startUs = currentPlayRange.startUs;
    decodeBufferRange.endUs = frame->getPtsUs() + frame->getDurationUs();

    bool useAudio = avSync->getMastClockType() == IAVMediaType::AUDIO;
    int64_t durationUs = useAudio ? mediaInfo.audioDurationUs : mediaInfo.videoDurationUs;

    // 回调上层
    callback->onBufferPosition(decodeBufferRange.startUs, decodeBufferRange.endUs, durationUs);

    logger.i("updateBufferRange start[%d]ms end[%d]ms",
             decodeBufferRange.startUs / 1000, decodeBufferRange.endUs / 1000);
}

void HelloPlayer::onPlayComplete()
{
    if (playerHandler)
    {
        playerHandler->post([](void *userdata) -> void {
            auto native = reinterpret_cast<HelloPlayer *>(userdata);

            if (native->playConfig.loopPlay)
            {
                native->seekTo(0, true);
            } else
            {
                native->updateState(Complete);
            }

        }, this);
    }
}


/**
 * 音视频编码数据加载回调，one by one
 * @param output 解复用拿出来的编码数据包
 * @param userdata 用户携带参数
 */
void HelloPlayer::onPacketRead(const std::shared_ptr<IAVPacket> &output, void *userdata)
{
    auto native = reinterpret_cast<HelloPlayer *>(userdata);
    Logger &logger = native->logger;

    bool flushBuffers = output->flushBuffers;
    int64_t startUs = output->getPtsUs();
    int64_t durationUs = output->getDurationUs();
    std::string typeStr;
    switch (output->type)
    {
        case AUDIO:
            typeStr += "AUDIO";
            native->audioDecoder->pushInputData(output);
            break;
        case VIDEO:
            typeStr += "VIDEO";
            native->videoDecoder->pushInputData(output);
            break;
        case UNKNOWN:
            typeStr += "UNKNOWN";
            break;
    }

    // seek end 触发，回调上层
    if (native->isSeeking())
    {
        native->playerHandler->post(HelloPlayer::onSeekEnd, native);
    }

    logger.i("onPacketRead[%s] packet.serial[%d] pts[%d]ms duration[%d]ms flush[%d]",
             typeStr.c_str(), output->serial, startUs / 1000, durationUs / 1000, flushBuffers);
}

/**
 * 音频编码数据包 送解 前的拦截器
 * @param packet 待送解 的编码数据包
 * @param userdata 用户携带参数
 */
bool HelloPlayer::OnBeforeAudioDecode(const std::shared_ptr<IAVPacket> &packet, void *userdata)
{
    auto native = reinterpret_cast<HelloPlayer *>(userdata);
    Logger &logger = native->logger;

    HelloClock &masterClock = native->avSync->getMasterClock();
    std::shared_ptr<HelloAudioPlayer> player = native->audioPlayer;
    if (player)
    {
        int limit = int(AudioFrameMaxCount * masterClock.speed);
        return player->getQueueSize() >= limit;
    }

    return false;
}

/**
 * 视频编码数据包 送解 前的拦截器
 * @param packet 待送解 的编码数据包
 * @param userdata 用户携带参数
 */
bool HelloPlayer::OnBeforeVideoDecode(const std::shared_ptr<IAVPacket> &packet, void *userdata)
{
    auto native = reinterpret_cast<HelloPlayer *>(userdata);
    Logger &logger = native->logger;

    HelloClock &masterClock = native->avSync->getMasterClock();
    std::shared_ptr<HelloVideoPlayer> player = native->videoPlayer;
    if (player)
    {
        int limit = int(VideoFrameMaxCount * masterClock.speed);
        return player->getQueueSize() >= limit;
    }

    return false;
}

/**
 * 音频编码数据 解码成功后回调
 * @param output 解码好的数据
 * @param userdata 用户携带参数
 */
void HelloPlayer::onAudioDecoded(const std::shared_ptr<IAVFrame> &output, void *userdata)
{
    auto native = reinterpret_cast<HelloPlayer *>(userdata);
    Logger &logger = native->logger;

    int64_t startUs = output->getPtsUs();
    int64_t durationUs = output->getDurationUs();

    // 是否为主时钟，更新缓冲区范围
    if (native->avSync->isMasterClock(IAVMediaType::AUDIO))
    {
        native->updateBufferRange(output);
    }

    // 送入音频，准备播放
    native->audioPlayer->pushInputData(output);

    logger.i("onAudioDecoded serial[%d] pts[%d]ms duration[%d]ms",
             output->serial, startUs / 1000, durationUs / 1000);
}

/**
 * 视频编码数据 解码成功后回调
 * @param output 解码好的数据
 * @param userdata 用户携带参数
 */
void HelloPlayer::onVideoDecoded(const std::shared_ptr<IAVFrame> &output, void *userdata)
{
    auto native = reinterpret_cast<HelloPlayer *>(userdata);
    Logger &logger = native->logger;

    int64_t startUs = output->getPtsUs();
    int64_t durationUs = output->getDurationUs();

    // 是否为主时钟，更新缓冲区范围
    if (native->avSync->isMasterClock(IAVMediaType::VIDEO))
    {
        native->updateBufferRange(output);
    }

    // 送入画面，准备播放
    native->videoPlayer->pushInputData(output);

    logger.i("onVideoDecode frame.serial[%d] pts[%d]ms duration[%d]ms",
             output->serial, startUs / 1000, durationUs / 1000);
}

/**
 * 音频数据播放完毕回调
 * @param output 播放完的数据
 * @param userdata 用户携带参数
 */
void HelloPlayer::onAudioRendered(const std::shared_ptr<IAVFrame> &output, void *userdata)
{
    auto native = reinterpret_cast<HelloPlayer *>(userdata);
    Hello::MediaInfo *mediaInfo = &native->mediaInfo;

//    Logger &logger = native->logger;
//    int64_t startUs = output->getPtsUs();
//    int64_t durationUs = output->getDurationUs();
//    logger.i("onAudioRendered pts[%d]ms duration[%d]ms", startUs / 1000, durationUs / 1000);

    // 判断是否为播放完毕
    if (output->eof && mediaInfo->audioDurationUs >= mediaInfo->videoDurationUs)
    {
        native->onPlayComplete();
    }

}

/**
 * 视频数据播放完毕回调
 * @param output 播放完的数据
 * @param userdata 用户携带参数
 */
void HelloPlayer::onVideoRendered(const std::shared_ptr<IAVFrame> &output, void *userdata)
{
    auto native = reinterpret_cast<HelloPlayer *>(userdata);
    Hello::MediaInfo *mediaInfo = &native->mediaInfo;

//    Logger &logger = native->logger;
//    int64_t startUs = output->getPtsUs();
//    int64_t durationUs = output->getDurationUs();
//    logger.i("onVideoRendered pts[%d]ms duration[%d]ms", startUs / 1000, durationUs / 1000);

    // 判断是否为播放完毕
    if (output->eof && mediaInfo->videoDurationUs >= mediaInfo->audioDurationUs)
    {
        native->onPlayComplete();
    }
}

/**
 * 主时钟更新回调
 * @param ptsUs 当前更新时间回调（变速后），单位：us
 * @param userdata 用户携带参数
 */
void HelloPlayer::onMasterClockUpdate(int64_t ptsUs, void *userdata)
{
    auto native = reinterpret_cast<HelloPlayer *>(userdata);

    Logger &logger = native->logger;
    std::shared_ptr<HelloAVSync> &avSync = native->avSync;
    Hello::MediaInfo &mediaInfo = native->mediaInfo;
    PlayConfig *config = &native->playConfig;
    PlayRange *bufferRange = &native->decodeBufferRange;
    PlayRange *playRange = &native->currentPlayRange;

    std::shared_ptr<HelloPlayerCallback> &callback = native->callback;

    // seek操作 加载数据 到 数据渲染，需要通知一下状态改变
    if (native->isSeeking())
    {
        return;
    }
    if (!native->isPlaying())
    {
        native->updateState(Playing);
    }

    // 回调上层
    bool useAudio = avSync->getMastClockType() == IAVMediaType::AUDIO;
    int64_t durationUs = useAudio ? mediaInfo.audioDurationUs : mediaInfo.videoDurationUs;
    int64_t adjustPtsUs = ptsUs;
    if (mediaInfo.isLiveStreaming)
    {
        durationUs = adjustPtsUs; // 直播流
    } else
    {
        adjustPtsUs = ptsUs - mediaInfo.startOffsetUs; // 有些多媒体文件有start偏移量
    }
    callback->onCurrentPosition(adjustPtsUs, durationUs);

    size_t audioDecoderSize = native->audioDecoder->getQueueSize();
    size_t videoDecoderSize = native->videoDecoder->getQueueSize();
    size_t audioPlayerSize = native->audioPlayer->getQueueSize();
    size_t videoPlayerSize = native->videoPlayer->getQueueSize();
    logger.i("Victor123 audioDecoder[%d] videoDecoder[%d] audioPlayer[%d] videoPlayer[%d]", audioDecoderSize,
             videoDecoderSize, audioPlayerSize, videoPlayerSize);

//    logger.i("on master clock update adjustPts[%d]ms useAudio[%d] duration[%d]ms",
//             adjustPtsUs / 1000, useAudio, durationUs / 1000);

    // 判断是否需要预加载数据
    // 先判断
    bool dataReading = bufferRange->endUs < playRange->endUs;
    if (dataReading)
    {
        // 数据正在加载中，暂时不判断，如果一直卡，需要检查数据读取部分时候有问题
        return;
    }

    // 这个水位线表示预加载时机
    int64_t bufferDurationUs = bufferRange->endUs - bufferRange->startUs;
    int64_t waterLevel = bufferRange->startUs + bufferDurationUs * config->bufferLoadMoreFactor;
    if (ptsUs >= waterLevel)
    {
        native->logger.i("preload next play range pts[%d]ms waterLevel[%d]ms", ptsUs / 1000,
                         waterLevel / 1000);
        // 加载更多数据
        if (native->loadNextPlayRange())
        {
            return;
        }
    }

}

/**
 * 重新恢复到空闲状态，随后可重新设置资源调用prepare
 * @param userdata 用户携带参数
 */
void HelloPlayer::onPlayerReset(void *userdata)
{
    auto native = reinterpret_cast<HelloPlayer *>(userdata);
    Logger &logger = native->logger;
    logger.i("HelloPlayer::onPlayerReset start");

    // 重置5大组件
    logger.i("native->dataReader->reset(); start");
    native->dataReader->reset();
    logger.i("native->dataReader->reset();");
    native->audioDecoder->reset();
    logger.i("native->audioDecoder->reset();");
    native->videoDecoder->reset();
    logger.i("native->videoDecoder->reset();");
    native->audioPlayer->reset();
    logger.i("native->audioPlayer->reset();");
    native->videoPlayer->reset();
    logger.i("native->videoPlayer->reset();");

    // 重置PlayRange
    native->currentPlayRange = PlayRange();
    native->decodeBufferRange = PlayRange();
    // 重置时间轴
    native->avSync->reset(0);
    native->avSync->setPaused(false);
    // 重置媒体信息
    native->mediaInfo = Hello::MediaInfo();

    // 更新播放器状态
    native->updateState(Idle);

    logger.i("HelloPlayer::onPlayerReset success");
}