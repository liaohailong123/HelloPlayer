//
// Created by liaohailong on 2025/1/11.
//

#include "HelloAudioPlayer.hpp"

#include <cstddef>
#include <cstdint>
#include <utility>

FILE *pcmFile = nullptr;


HelloAudioPlayer::HelloAudioPlayer(std::shared_ptr<HelloAVSync> avSync) :
        HelloProcessor("HelloAudioPlayer"), prepared(false), mutex(),
        properties(nullptr), renderProperties(nullptr), render(nullptr),
        avSync(std::move(avSync)), audioController(nullptr),
        audioControllerChanged(false), volume(1.0), speed(1.0), lastFrame(nullptr)
{
    logger.i("HelloAudioPlayer::HelloAudioPlayer(%p)", this);

//    pcmFile = fopen(
//            "/storage/emulated/0/Android/data/com.example.helloplayer/files/mp4/48000_6_f32le.pcm",
//            "wb");
    // Ohos测试 /data/storage/el2/base/haps/entry/files
//    pcmFile = fopen(
//            "/data/storage/el2/base/haps/entry/files/44100_2_s16le.pcm",
//            "wb");
    if (pcmFile)
    {
        logger.i("pcmFile opened");
    }
}

HelloAudioPlayer::~HelloAudioPlayer()
{
    reset();

    logger.i("HelloAudioPlayer::~HelloAudioPlayer(%p)", this);
}

void HelloAudioPlayer::prepare(const std::shared_ptr<AudioProperties> &p, PlayConfig *config)
{
    // 勿重复设置
    if (render || prepared)return;

    properties = p;

    render = HelloDeviceRegister::onCreateHelloAudioRender();
    renderProperties = render->setConfig(properties);
    render->setFramesPerCallback(properties->frame_size);
    render->setCallback(HelloAudioPlayer::onDataCallback, this);

    // 设置音量与倍速，默认值
    setVolume(config->volume);
    setSpeed(config->speed);

    logger.i("player->openStreams() start");
    bool success = render->start();
    logger.i("player.openStreams() -> %d volume[%f] speed[%f]",
             success, volume, speed);

    // 重要：判断是否需要重采样，否则音频渲染器可能播放异常
    audioControllerChanged = needResample();
    prepared = true;
}

void HelloAudioPlayer::setVolume(double _volume)
{
    if (volume == _volume)return;
    volume = _volume;
    if (audioController)
    {
        audioController->setVolume(_volume);
    } else
    {
        audioControllerChanged = true;
    }

    logger.i("setVolume[%f]", _volume);
}

bool HelloAudioPlayer::isDefaultVolume() const
{
    return volume == 1.0;
}

/**
 * @param speed  音频速率，默认1.0 范围[0.5,100]
 */
void HelloAudioPlayer::setSpeed(double _speed)
{
    if (speed == _speed)return;
    speed = _speed;
    // 速度改变需要重新创建，为了重新获取 firstFramePts
    audioControllerChanged = true;
    logger.i("setSpeed[%f]", _speed);
}

bool HelloAudioPlayer::isDefaultSpeed() const
{
    return speed == 1.0;
}

/**
 * @return true表示需要重采样
 */
bool HelloAudioPlayer::needResample()
{
    if(properties && renderProperties)
    {
        // 采样率不同？
        if(properties->sampleRate != renderProperties->sampleRate) return true;
        // 声道数不同？
        int channelCount1 = properties->channelCount;
        int channelCount2 = renderProperties->channelCount;
        if(channelCount1 != channelCount2) return true;
        // 位深不同？
        if(properties->sampleFmt != renderProperties->sampleFmt) return true;
    }
    return false;
}


void HelloAudioPlayer::ensureAudioControllerIfNecessary(const std::shared_ptr<IAVFrame>& frame)
{
    // 判断是否需要重制音频控制器
    if (audioControllerChanged)
    {
        audioControllerChanged = false;
        // 释放旧资源
        releaseAudioController();

        // 根据实际情况，判断是否需要创建
        if (!isDefaultSpeed() || !isDefaultVolume() || needResample())
        {
            audioController = std::make_shared<AudioController>();
            // 输入信息
            audioController->setAudioInput(
                    frame->frame->time_base,
                    properties->sampleRate,
                    frame->frame->ch_layout,
                    (AVSampleFormat) frame->frame->format);
            // 输出信息
            AVChannelLayout channel_layout;
            int nb_channels = renderProperties->channelCount;
            int ret = av_channel_layout_custom_init(&channel_layout,nb_channels);
            if (ret < 0)
            {
                FFUtil::av_print_error(logger,ret,"创建 AVChannelLayout 失败");
                audioController = nullptr;
                return;
            }
            audioController->setAudioOutput(
                    renderProperties->sampleRate,
                    channel_layout,
                    (AVSampleFormat) renderProperties->sampleFmt);
            audioController->setVolume(volume);
            audioController->setSpeed(speed);
            audioController->init();

            logger.i("audioController create success format[%d] -> format[%d]",
                     frame->frame->format,renderProperties->sampleFmt);
        }

    }
}

void HelloAudioPlayer::releaseAudioController()
{
    audioController = nullptr;
    lastFrame = nullptr;
}

void HelloAudioPlayer::closeAudioRender()
{
    if (render != nullptr)
    {
        render->stop();
        render = nullptr;
    }
}

bool HelloAudioPlayer::onProcess(std::shared_ptr<InputDataCtx> inputData)
{
    // 数据都在缓冲队列之中，这里不做处理，在 [onDataCallback] 中消费掉

    return false;
}

int64_t addFilterCount = 0;

void HelloAudioPlayer::onDataCallback(HelloAudioRender *render,
                                      void *audioData, int32_t numFrames, void *userdata)
{

    int32_t bytesPerFrame = render->getBytesPerFrame();
    int32_t totalBytesCount = bytesPerFrame * numFrames;

    auto native = reinterpret_cast<HelloAudioPlayer *>(userdata);

    // 修改资源操作，需挂锁,onReset函数已经持有锁,这里就终止操作
    if (!native->mutex.try_lock()) {
        return;
    }
    std::unique_lock<std::mutex> locker(native->mutex, std::adopt_lock);

    Logger &logger = native->logger;
    std::shared_ptr<HelloAVSync> &avSync = native->avSync;
    bool isMasterClock = avSync->isMasterClock(IAVMediaType::AUDIO);
    HelloClock &masterClock = avSync->getMasterClock();


    // 本次默认静音
    memset(audioData, 0x00, totalBytesCount);

    // 音频存在，就是主时钟
    if (!isMasterClock)
    {
        logger.i("Audio clock should be master clock");
        return;
    }


    // 最后一次送播的音频数据
    std::shared_ptr<IAVFrame> frame;
    int64_t timestampsUs = av_gettime_relative();

    // 本次数据送入，记录时间
    int64_t audioClock = 0;
    // 本次写入了多少样本
    int64_t writeSamples = 0;
    int64_t writeOffset = 0;

    while (numFrames > 0)
    {
        // 首先判断空数据的情况
        if (native->getQueueSize() <= 0)
        {
//            logger.i("Queue is empty and keep silent filter[%d]", addFilterCount);
            return;
        }

        // 取出一帧音频数据
        std::shared_ptr<InputDataCtx> inputData = native->peekFirst();
        frame = inputData->data;

        // 序列号不对了，丢弃该包
        if (masterClock.serial != frame->serial)
        {
            int64_t masterSerial = masterClock.serial;
            // 音频数据解码快，这里缓存全清没事
            int64_t count = native->clearQueue();
            logger.i("masterClock.serial[%d] != frame->serial[%d] count[%d]",
                     masterSerial, frame->serial, count);
            // 重置AudioController
            native->audioControllerChanged = true;
            return;
        }

        if (frame->eof)
        {
            logger.i("receive eof");
            // 回调出去，外部可能会做对应逻辑处理
            native->sendCallback(frame);
            return;
        }

        // 暂停了
        if (masterClock.isPaused())
        {
//            logger.i("master clock is paused and keep silent");
            return;
        }

        // 按需创建 音频处理器
        native->ensureAudioControllerIfNecessary(frame);

        std::shared_ptr<HelloAudioPlayer::FillDataResult> result
                = onFillData(frame, audioData, writeOffset, numFrames, userdata);
        int64_t samples = result->samples;
        frame = result->frame; // 更新frame，onFillData中有可能是从音频过滤器中取出来的历史frame
        // 本次数据送入，记录时间
        if (frame)
        {
            audioClock = frame->getPtsUs(); // AudioController中把pts改成了变速时间
        }

        numFrames -= static_cast<int32_t>(samples);
        writeSamples += samples;
        writeOffset += (bytesPerFrame * samples);
    }

    // 测试代码
    if (pcmFile)
    {
        fwrite(audioData, 1, totalBytesCount, pcmFile);
    }

    // 更新当前时钟，方便下次判断时间差
    int samplerRate = native->properties->sampleRate;
    int64_t durationUs = (writeSamples * 1000000) / samplerRate;
    int64_t delayFrameCount = render->getDelayFrameCount(); // 音频渲染器内部缓冲帧数(播放延迟)
    int64_t delayUs = (delayFrameCount * 1000000) / samplerRate;
    audioClock -= delayUs; // 矫正当前音频时钟,处理音频渲染器延迟播放问题
    audioClock = FFMAX(audioClock,0);
    masterClock.setPtsUsAt(audioClock, durationUs, timestampsUs);
//    int64_t adjustPtsUs = audioClock * native->speed;
//    logger.i("AVPlayer[audio] pts[%d]ms duration[%d]ms",
//             adjustPtsUs / 1000, durationUs / 1000);
    // 数据回调外部消费者
    native->sendCallback(frame);
}

std::shared_ptr<HelloAudioPlayer::FillDataResult>
HelloAudioPlayer::onFillData(const std::shared_ptr<IAVFrame>& frame,
                             void *audioData, int64_t writeOffset,
                             int32_t numFrames, void *userdata)
{
    auto native = reinterpret_cast<HelloAudioPlayer *>(userdata);
    Logger &logger = native->logger;

    std::shared_ptr<HelloAudioPlayer::FillDataResult> output = std::make_shared<HelloAudioPlayer::FillDataResult>();

    // 判断音频数据是否需要经过过滤器处理
    if (native->audioController)
    {
        std::shared_ptr<AudioController> &audioController = native->audioController;

        // 存在上次缓存数据，先写入
        if (native->lastFrame)
        {
            int samples = native->lastFrame->readAudioSamples(audioData, writeOffset,
                                                              numFrames);
            output->frame = native->lastFrame;
            output->samples = samples;

            if (samples <= 0)
            {
                // 读取完毕了
                native->lastFrame = nullptr;
            }
        } else
        {
            AVFrame *filterFrame = audioController->getFrame();

            if (filterFrame)
            {

                addFilterCount--;
                native->lastFrame = std::make_shared<IAVFrame>();

                // 转移数据
                native->lastFrame->move(filterFrame);
                av_frame_free(&filterFrame);
                filterFrame = nullptr;

//                int64_t ptsUs = native->lastFrame->getPtsUs();
//                logger.i("audio controller get frame pts[%d]ms remain[%d]",
//                         ptsUs / 1000, addFilterCount);

//                logger.i("get lastFrame pts[%d]ms",native->lastFrame->getPtsUs()/1000);
                // 填充数据到 obo stream 中
                int samples = native->lastFrame->readAudioSamples(audioData, writeOffset,
                                                                  numFrames);
                output->frame = native->lastFrame;
                output->samples = samples;

                if (samples <= 0)
                {
                    logger.i("new filter frame received but read zero sample");
                    // 读取完毕了
                    native->lastFrame = nullptr;
                }
            } else
            {
                int64_t ptsUs = frame->getPtsUs();
                bool add = audioController->addFrame(frame->frame);
                if (add)
                {
//                    logger.i("audio controller add frame pts[%d]ms", ptsUs / 1000);
                    addFilterCount++;
                } else
                {
                    logger.i("Cannot add frame to audioController");
                }
                // 从缓冲队列中移除
                native->popFirst();

                output->frame = nullptr;
                output->samples = 0;
            }
        }
    } else
    {
        // 音频播放配置正常，不需要使用 过滤器
        // 直接把数据填充到 音频设备buffer上
        int samples = frame->readAudioSamples(audioData, writeOffset, numFrames);
        if (samples != numFrames)
        {
            logger.i("fill samples not match samples[%d] != numFrames[%d]",
                     samples, numFrames);
        } else
        {
//            logger.i("fill samples success samples[%d]", samples);
        }
        // 从缓冲队列中移除
        native->popFirst();

        output->frame = frame;
        output->samples = numFrames;
    }

    return output;
}

void HelloAudioPlayer::onReset()
{
    prepared = false;

    // 修改资源操作，需挂锁
    std::unique_lock<std::mutex> locker(mutex);

    volume = speed = 1.0;

    closeAudioRender();
    releaseAudioController();
}
