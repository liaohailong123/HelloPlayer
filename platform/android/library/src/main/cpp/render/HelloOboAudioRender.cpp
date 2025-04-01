//
// Created by 廖海龙 on 2025/2/28.
//

#include "HelloOboAudioRender.hpp"

HelloOboAudioRender::HelloOboAudioRender() : HelloAudioRender("HelloOboAudioRender"),
                                             framesPerCallback(1024), callback(nullptr),
                                             userdata(nullptr), mPlayStream(nullptr)
{
    logger.i("HelloOboAudioRender::HelloOboAudioRender(%p)", this);
}

HelloOboAudioRender::~HelloOboAudioRender()
{
    logger.i("HelloOboAudioRender::~HelloOboAudioRender(%p)", this);
}

std::shared_ptr<AudioProperties> HelloOboAudioRender::setConfig(std::shared_ptr<AudioProperties> p)
{
    std::shared_ptr<AudioProperties> support = std::make_shared<AudioProperties>();
    support->sampleRate = p->sampleRate;
    support->channelCount = p->channelCount;
    support->sampleFmt = p->sampleFmt;
    support->frame_size = p->frame_size;

    int sampleRate = p->sampleRate;
    int channelCount = p->channelCount;
    oboe::AudioFormat format = oboe::AudioFormat::Invalid;
    int frameSize = p->frame_size; // 一帧音频包含多少个样本

    // 支持多类型声道
    oboe::ChannelMask channelMask = getOboeChannelMask(channelCount);

    if (p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_S16 ||
        p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_S16P)
    {
        format = oboe::AudioFormat::I16;
        logger.i("sampleFmt: oboe::AudioFormat::I16");
    } else if (p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_S32 ||
               p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_S32P)
    {
        format = oboe::AudioFormat::I32;
        logger.i("sampleFmt: oboe::AudioFormat::I32");
    } else if (p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_FLT ||
               p->sampleFmt == AVSampleFormat::AV_SAMPLE_FMT_FLTP)
    {
        format = oboe::AudioFormat::Float;
        logger.i("sampleFmt: oboe::AudioFormat::Float");
    }

    // 判断声道是否支持
    if(oboe::ChannelMask::Unspecified == channelMask)
    {
        // 默认返回 双声道，外部需要重采样
        support->channelCount = 2;
        channelMask = oboe::ChannelMask::Stereo;
        logger.i("channelMask(default): oboe::ChannelMask::Stereo");
    }

    // 判断位深是否支持
    if (oboe::AudioFormat::Invalid == format)
    {
        // 默认返回 16位整型PCM数据，外部需要重采样
        support->sampleFmt = AVSampleFormat::AV_SAMPLE_FMT_S16;
        format = oboe::AudioFormat::I16;
        logger.i("sampleFmt(default): oboe::AudioFormat::I16");
    }

    logger.i("sampleRate[%d] channelCount[%d] format[%d] frameSize[%d]",
             sampleRate, channelCount, support->sampleFmt, frameSize);

    mAudioApi = oboe::AudioApi::AAudio;
    mSampleRate = sampleRate;
    mChannelMask = channelMask;
    mFormat = format;

    return support;
}

void HelloOboAudioRender::setFramesPerCallback(int _framesPerCallback)
{
    this->framesPerCallback = _framesPerCallback;
    logger.i("setFramesPerCallback: %d", _framesPerCallback);
}

void HelloOboAudioRender::setCallback(Callback _callback, void *_userdata)
{
    this->callback = _callback;
    this->userdata = _userdata;
}

bool HelloOboAudioRender::start()
{
    logger.i("HelloOboAudioRender::start(%p)",this);
    // Note: The order of stream creation is important. We create the playback
    // stream first, then use properties from the playback stream
    // (e.g. sample rate) to create the recording stream. By matching the
    // properties we should get the lowest latency path
    oboe::AudioStreamBuilder outBuilder;
    setupPlaybackStreamParameters(&outBuilder);
    oboe::Result result = outBuilder.openStream(mPlayStream);

    if (result != oboe::Result::OK)
    {
        logger.e("Failed to open output stream. Error %s", oboe::convertToText(result));
        mSampleRate = oboe::kUnspecified;
        return false;
    } else
    {
        // The input stream needs to run at the same sample rate as the output.
        mSampleRate = mPlayStream->getSampleRate();
    }
    warnIfNotLowLatency(mPlayStream);

    mPlayStream->requestStart();
    logger.i("HelloOboAudioRender::openStreams opened");
    return true;
}

int32_t HelloOboAudioRender::getBytesPerFrame()
{
    if (mPlayStream)
    {
        return mPlayStream->getBytesPerFrame();
    }
    return 0;
}

void HelloOboAudioRender::stop()
{
    closeStreams();
}

void HelloOboAudioRender::closeStreams()
{
    if (mPlayStream)
    {
        mPlayStream->requestStop();
        closeStream(mPlayStream);
    }
}

void HelloOboAudioRender::closeStream(std::shared_ptr<oboe::AudioStream> &stream)
{
    if (stream)
    {
        oboe::Result result = stream->stop();
        if (result != oboe::Result::OK)
        {
            logger.e("Error stopping stream: %s", oboe::convertToText(result));
        }
        result = stream->close();
        if (result != oboe::Result::OK)
        {
            logger.e("Error closing stream: %s", oboe::convertToText(result));
        } else
        {
            logger.i("Successfully closed streams");
        }
        stream.reset();
    }
}

oboe::ChannelMask HelloOboAudioRender::getOboeChannelMask(int nb_channels)
{

    logger.i("getOboeChannelMask AV_CH_LAYOUT_MONO[%d]", AV_CH_LAYOUT_MONO);
    logger.i("getOboeChannelMask AV_CH_LAYOUT_STEREO[%d]", AV_CH_LAYOUT_STEREO);
    logger.i("getOboeChannelMask AV_CH_LAYOUT_5POINT1[%d]", AV_CH_LAYOUT_5POINT1);

    logger.i("getOboeChannelMask oboe::ChannelMask::Mono[%d]", oboe::ChannelMask::Mono);
    logger.i("getOboeChannelMask oboe::ChannelMask::Stereo[%d]", oboe::ChannelMask::Stereo);
    logger.i("getOboeChannelMask oboe::ChannelMask::CM5Point1[%d]", oboe::ChannelMask::CM5Point1);

    if (nb_channels == 1)
    {
        return oboe::ChannelMask::Mono;
    } else if (nb_channels == 2)
    {
        return oboe::ChannelMask::Stereo;
    } else if (nb_channels == 3)
    {
        return oboe::ChannelMask::CM2Point1;
    } else if (nb_channels == 6)
    {
        return oboe::ChannelMask::CM5Point1;
    } else if (nb_channels == 8)
    {
        return oboe::ChannelMask::CM7Point1;
    } else
    {
        return oboe::ChannelMask::Unspecified;  // 对于无法映射的布局，返回Unspecified
    }
}


oboe::AudioStreamBuilder *HelloOboAudioRender::setupCommonStreamParameters(
        oboe::AudioStreamBuilder *builder)
{
    logger.i("setupCommonStreamParameters enter");
    // We request EXCLUSIVE mode since this will give us the lowest possible
    // latency.
    // If EXCLUSIVE mode isn't available the builder will fall back to SHARED
    // mode.
    builder->setAudioApi(mAudioApi)
            ->setFormat(mFormat)
            ->setFormatConversionAllowed(true)
            ->setSharingMode(oboe::SharingMode::Exclusive)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency);
    logger.i("setupCommonStreamParameters end");
    return builder;
}

oboe::AudioStreamBuilder *HelloOboAudioRender::setupPlaybackStreamParameters(
        oboe::AudioStreamBuilder *builder)
{
    logger.i("Victor mPlaybackDeviceId: %d", mPlaybackDeviceId);
    builder->setDataCallback(this)
            ->setErrorCallback(this)
            ->setDeviceId(mPlaybackDeviceId)
            ->setDirection(oboe::Direction::Output)
            ->setSampleRate(mSampleRate)
            ->setChannelMask(mChannelMask)
            ->setFramesPerCallback(framesPerCallback)
            ->setContentType(oboe::Movie);

    return setupCommonStreamParameters(builder);
}

void HelloOboAudioRender::warnIfNotLowLatency(std::shared_ptr<oboe::AudioStream> &stream)
{
    if (stream->getPerformanceMode() != oboe::PerformanceMode::LowLatency)
    {
        logger.e(
                "Stream is NOT low latency."
                "Check your requested format, sample rate and channel count");
    }
}

/*
 * oboe::AudioStreamDataCallback interface implementation
 */
oboe::DataCallbackResult HelloOboAudioRender::onAudioReady(oboe::AudioStream *oboeStream,
                                                      void *audioData, int32_t numFrames)
{
    auto *outputBuffer = static_cast<uint8_t *>(audioData);

    int32_t bytesPerFrame = oboeStream->getBytesPerFrame();
    int32_t totalSize = bytesPerFrame * numFrames;

    if (callback)
    {
        callback(this, audioData, numFrames, userdata);
    } else {
        // 默认静音
        memset(outputBuffer, 0x00, totalSize);
    }

    // 测一下音频缓冲区大小
//    oboeStream->getBufferCapacityInFrames();
//    int64_t framePosition; /* framePosition */
//    int64_t timeNanoseconds; /* timeNanoseconds */
//    oboe::Result ret = oboeStream->getTimestamp(CLOCK_MONOTONIC, &framePosition, &timeNanoseconds);
//    if (ret == oboe::Result::OK) {
//        logger.i("oboeStream->getTimestamp framePosition[%ld] timeNanoseconds[%ld] frameSize[%d]",
//                 framePosition, timeNanoseconds, numFrames);
//    } else {
//        logger.i("oboeStream->getTimestamp error");
//    }

//    logger.i("OboAudioPlayer::onAudioReady[%d]", numFrames);

    return oboe::DataCallbackResult::Continue;
}
