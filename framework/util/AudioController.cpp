//
// Created by 廖海龙 on 2025/1/16.
//

#include "AudioController.hpp"

#include <memory>

AudioController::AudioController() : logger("AudioController"), initialized(false), mutex(),
                                     audioInputInfo(nullptr), audioVolumeInfo(nullptr),
                                     audioRateInfo(nullptr), audioOutputInfo(nullptr),
                                     audioSinkInfo(nullptr), filterGraph(nullptr),
                                     rate(1.0), volume(1.0), firstFramePts(-1)
{
    logger.i("AudioController::AudioController(%p)", this);
}

AudioController::~AudioController()
{
    release();
    logger.i("AudioController:~:AudioController(%p)", this);
}

bool AudioController::setAudioInput(AVRational timeBase, uint32_t sampleRate,
                                    AVChannelLayout chanelLayout,
                                    AVSampleFormat format)
{
    // 挂锁，守护 initialized 关键字
    std::unique_lock<std::mutex> locker(mutex);
    if (initialized)return false; // 初始化之后就不能修改了

    audioInputInfo = std::make_shared<AudioInfo>();
    audioInputInfo->timeBase = timeBase;
    audioInputInfo->sampleRate = sampleRate;
    audioInputInfo->chanelLayout = chanelLayout;
    audioInputInfo->bitsPerSample = av_get_bytes_per_sample(format);
    audioInputInfo->format = format;
    audioInputInfo->name = "input";

    return true;
}

bool AudioController::setAudioOutput(uint32_t sampleRate, AVChannelLayout chanelLayout,
                                     AVSampleFormat format)
{
    // 挂锁，守护 initialized 关键字
    std::unique_lock<std::mutex> locker(mutex);
    if (initialized)return false; // 初始化之后就不能修改了

    audioOutputInfo = std::make_shared<AudioInfo>();
    audioOutputInfo->sampleRate = sampleRate;
    audioOutputInfo->chanelLayout = chanelLayout;
    audioOutputInfo->bitsPerSample = av_get_bytes_per_sample(format);
    audioOutputInfo->format = format;
    audioOutputInfo->name = "output";

    return true;
}

void AudioController::setVolume(double value)
{
    // 挂锁，守护 initialized 关键字
    std::unique_lock<std::mutex> locker(mutex);
    volume = value;
    if (!initialized || !filterGraph)return;

    std::string valueStr = std::to_string(value);
    int ret = avfilter_graph_send_command(filterGraph, "volume", "volume", valueStr.c_str(),
                                          nullptr, 0, 0);
    if (ret < 0)
    {
        logger.i("set volume error");
    }
}

void AudioController::setSpeed(double value)
{
    // 挂锁，守护 initialized 关键字
    std::unique_lock<std::mutex> locker(mutex);
    rate = value;
    if (!initialized || !filterGraph)return;

    std::string valueStr = std::to_string(value);
    int ret = avfilter_graph_send_command(filterGraph, "atempo", "tempo", valueStr.c_str(), nullptr,
                                          0, 0);
    if (ret < 0)
    {
        logger.i("set speed error");
    }
}

bool AudioController::init()
{
    // 挂锁，守护 initialized 关键字
    std::unique_lock<std::mutex> locker(mutex);
    if (initialized)return true; // 已经初始化过了，无需继续初始化，否则先调用release()

    if (!audioInputInfo)
    {
        logger.i("Input audio info not set");
        return false;
    }

    if (!audioOutputInfo)
    {
        logger.i("Ouput audio info not set");
        return false;
    }

    // 创建 FilterGraph
    filterGraph = avfilter_graph_alloc();
    if (!filterGraph)
    {
        logger.i("Cannot alloc filter graph!");
        return false;
    }

    char args[1024] = {0};

    // 过滤器：Sink
    audioSinkInfo = std::make_shared<AudioInfo>();
    audioSinkInfo->name = "sink";
    audioSinkInfo->sampleRate = audioOutputInfo->sampleRate;
    audioSinkInfo->chanelLayout = audioOutputInfo->chanelLayout;
    audioSinkInfo->bitsPerSample = audioOutputInfo->bitsPerSample;
    audioSinkInfo->format = audioOutputInfo->format;
    audioSinkInfo->filterCtx = createFilter("abuffersink", args);

    // 过滤器：音量大小
    snprintf(args, sizeof(args), "volume=%f", volume);
    audioVolumeInfo = std::make_shared<AudioInfo>();
    audioVolumeInfo->name = "volume";
    audioVolumeInfo->volume = volume;
    audioVolumeInfo->filterCtx = createFilter("volume", args);

    // 过滤器：播放速度
    snprintf(args, sizeof(args), "tempo=%f", rate);
    audioRateInfo = std::make_shared<AudioInfo>();
    audioRateInfo->name = "atempo";
    audioRateInfo->rate = rate;
    audioRateInfo->filterCtx = createFilter("atempo", args);


    // 初始化输入过滤器
    int ret = snprintf(args, sizeof(args),
                       "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=",
                       audioInputInfo->timeBase.num, audioInputInfo->timeBase.den,
                       audioInputInfo->sampleRate,
                       av_get_sample_fmt_name(audioInputInfo->format));
    av_channel_layout_describe(&audioInputInfo->chanelLayout, args + ret, sizeof(args) - ret);
    logger.i("create abuffer: %s", args);
    audioInputInfo->filterCtx = createFilter("abuffer", args);

    // 初始化重采样
    bool notSamplerRate = audioInputInfo->sampleRate != audioOutputInfo->sampleRate;
    bool notChannels =
            audioInputInfo->chanelLayout.nb_channels != audioOutputInfo->chanelLayout.nb_channels;
    bool notFormat = audioInputInfo->format != audioOutputInfo->format;
    // 音频基本信息有一个不一样就需要进行重采样操作
    if (notSamplerRate || notChannels || notFormat)
    {
        ret = snprintf(args, sizeof(args),
                 "sample_rates=%d:sample_fmts=%s:channel_layouts=",
                 audioOutputInfo->sampleRate,
                 av_get_sample_fmt_name(audioOutputInfo->format));
        av_channel_layout_describe(&audioOutputInfo->chanelLayout, args + ret, sizeof(args) - ret);
        logger.i("create aformat: %s", args);
        audioOutputInfo->filterCtx = createFilter("aformat", args);
    }


    // 把所有过滤器全部连接起来
    if (avfilter_link(audioInputInfo->filterCtx, 0, audioVolumeInfo->filterCtx, 0) != 0)
    {
        logger.i("error: avfilter_link(%s,%s)", "input", "volumn");
        return false;
    }
    if (avfilter_link(audioVolumeInfo->filterCtx, 0, audioRateInfo->filterCtx, 0) != 0)
    {
        logger.i("error: avfilter_link(%s,%s)", "volumn", "atempo");
        return false;
    }

    // 判断一下可能没有重采样的情况
    AVFilterContext *current;
    const char *name;
    if (audioOutputInfo->filterCtx)
    {
        if (avfilter_link(audioRateInfo->filterCtx, 0, audioOutputInfo->filterCtx, 0) != 0)
        {
            logger.i("error: avfilter_link(%s,%s)", "atempo", "output");
            return false;
        }
        current = audioOutputInfo->filterCtx;
        name = "output";
    } else
    {
        current = audioRateInfo->filterCtx;
        name = "atempo";
    }
    if (avfilter_link(current, 0, audioSinkInfo->filterCtx, 0) != 0)
    {
        logger.i("error: avfilter_link(%s,%s)", name, "sink");
        return false;
    }

    if (avfilter_graph_config(filterGraph, nullptr) < 0)
    {
        logger.i("error: avfilter_graph_config()");
        return false;
    }


    initialized = true;
    return true;
}

bool AudioController::release()
{
    // 挂锁，守护 initialized 关键字
    std::unique_lock<std::mutex> locker(mutex);

    if (initialized)
    {
        initialized = false;

        audioInputInfo.reset();
        audioVolumeInfo.reset();
        audioRateInfo.reset();
        audioOutputInfo.reset();
        audioSinkInfo.reset();

        if (filterGraph)
        {
            avfilter_graph_free(&filterGraph);
            filterGraph = nullptr;
        }

        firstFramePts = -1;
    }

    return true;
}

bool AudioController::addFrame(AVFrame *frame)
{
    // 挂锁，守护 initialized 关键字
    std::unique_lock<std::mutex> locker(mutex);
    if (!initialized) return false;

    // 判断是否有数据
    if (frame && (!frame->buf[0] || frame->nb_samples <= 0))
    {
        frame = nullptr; // 判断是否为空包，冲刷缓冲区
    }
    if (frame && firstFramePts < 0)
    {
        firstFramePts = frame->pts;
    }

    int ret = av_buffersrc_add_frame(audioInputInfo->filterCtx, frame);
    if (ret != 0)
    {
        FFUtil::av_print_error(logger, ret, "av_buffersrc_add_frame error");
        return false;
    }

    return true;
}

AVFrame *AudioController::getFrame()
{
    // 挂锁，守护 initialized 关键字
    std::unique_lock<std::mutex> locker(mutex);
    if (!initialized) return nullptr;

    AVFrame *avFrame = av_frame_alloc();

    int ret = av_buffersink_get_frame(audioSinkInfo->filterCtx, avFrame);

    if (ret < 0)
    {
        av_frame_free(&avFrame); // 添加这行
//        FFUtil::av_print_error(logger, ret, "av_buffersink_get_frame error");
        return nullptr;
    }

    // 矫正frame中的pts：
    // 0，frame中的时间基是 1/采样率
    // 1，av_filter 变速改变的是样本数量
    // 2，会以 first pts 为基数，一直添加 nb_samples
    // 3，first pts 未变速，但是 nb_samples 是变速之后变少了的
    // 所以，把 first_pts / speed 之后，再加上 nb_samples 就是整个变速后的pts了
    // nb_samples = curr_pts - first_pts
    // real_pts = (first_pts / speed) + nb_samples
    AVRational filterTimeBase = av_buffersink_get_time_base(audioSinkInfo->filterCtx);
    int64_t pts = av_rescale_q(avFrame->pts, filterTimeBase, audioInputInfo->timeBase);
    int64_t nb_samples = pts - firstFramePts;
    double speed = rate;
    int64_t adjustPts = (firstFramePts / speed) + nb_samples;
    avFrame->pts = adjustPts;
//    logger.i("pts[%d] - firstFramePts[%d] = nb_samples[%d] speed[%f]",
//             pts, firstFramePts, nb_samples, speed);

    return avFrame;
}

AVFilterContext *AudioController::createFilter(const char *name, const char *opt)
{
    if (!filterGraph)
    {
        return nullptr;
    }
    const AVFilter *filter = avfilter_get_by_name(name);
    AVFilterContext *filterContext = avfilter_graph_alloc_filter(filterGraph, filter, name);
    if (!filterContext)
    {
        logger.i("filterContext is nullptr: %s", name);
        return nullptr;
    }
    int err = avfilter_init_str(filterContext, opt);
    if (err < 0)
    {
        logger.i("Could not initialize the %s filter.", opt);
        avfilter_free(filterContext);
        return nullptr;
    }

    return filterContext;
}

