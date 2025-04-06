//
// Created by liaohailong on 2025/1/12.
//

#include "HelloVideoPlayer.hpp"

#include <utility>

//FILE *yuvFile;

HelloVideoPlayer::HelloVideoPlayer(std::shared_ptr<HelloAVSync> avSync) :
        HelloProcessor("HelloVideoPlayer"), prepared(false), mutex(), properties(nullptr),
        render(nullptr), lastFrame(nullptr), waitForAdd(), waitForRemove(), avSync(std::move(avSync)),
        videoConverter(new VideoConverter())
{
    // Ohos测试 /data/storage/el2/base/haps/entry/files
//    yuvFile = fopen("/data/storage/el2/base/haps/entry/files/yuv420p.yuv", "wb");
//    if (yuvFile) {
//        logger.i("yuvFile opened");
//    }
    logger.i("HelloVideoPlayer::HelloVideoPlayer(%p)", this);
}

HelloVideoPlayer::~HelloVideoPlayer()
{
    reset();
    
    // 释放渲染器
    if (render) {
        render = nullptr;
    }

    logger.i("HelloVideoPlayer::~HelloVideoPlayer(%p)", this);
}

void HelloVideoPlayer::prepare(const std::shared_ptr<VideoProperties> &p, const PlayConfig &_config)
{
    this->properties = p;
    this->config = _config;

    if (handler)
    {
        handler->postAtFrontOfQueue(&HelloVideoPlayer::initRender, this);
    } else
    {
        logger.i("prepare failure cause handler is nullptr");
    }
}

void HelloVideoPlayer::initRender(void *userdata)
{
    auto native = reinterpret_cast<HelloVideoPlayer *>(userdata);
    const std::shared_ptr<VideoProperties> &p = native->properties;

    auto format = static_cast<AVPixelFormat>(p->colorFormat);
    if (native->render)
    {
        native->render->setAVPixelFormat(format);
        native->logger.i("render is already exist");
    } else
    {
        // 视频渲染的配置信息
        HelloVideoRender::VideoRenderCtx ctx = {
                .format = format, // 像素格式
                .config = native->config // 图形Api
        };
        native->render = HelloDeviceRegister::onCreateHelloVideoRender(ctx);
    }
    native->logger.i("create render success");

    // 借机添加surface
    native->addWaitingSurface();

    native->prepared = true;
}

void HelloVideoPlayer::addSurface(void *surface)
{
    logger.i("wait for add surface");
    waitForAdd.push_back(surface);

    if (handler)
    {
        logger.i("handler post to call addWaitingSurface");
        handler->postAtFrontOfQueue([](void *userdata) -> void {
            auto native = reinterpret_cast<HelloVideoPlayer *>(userdata);
            native->addWaitingSurface();
        }, this);
    }
}

void HelloVideoPlayer::removeSurface(void *surface)
{
    waitForRemove.push_back(surface);
    if (handler)
    {
        handler->postAtFrontOfQueue([](void *userdata) -> void {
            auto native = reinterpret_cast<HelloVideoPlayer *>(userdata);
            if (native->render)
            {
                for (void *s: native->waitForRemove)
                {
                    native->render->removeOutputs(s);
                }
                native->waitForRemove.clear();

                native->logger.i("canvas remove surface success");
            }
        }, this);
    }
}

void HelloVideoPlayer::addWaitingSurface()
{
    logger.i("addWaitingSurface start %p", render.get());
    if (render)
    {
        for (void *s: waitForAdd)
        {
            render->addOutputs(s);
            logger.i("render add surface(%p)", s);
        }
        waitForAdd.clear();
        logger.i("render add surface over");

        // 添加输出surface后才能初始化，内部有只初始化一次的判断
        render->prepare();
        if (render->isPrepared())
        {
            logger.i("render prepare success");
        } else
        {
            logger.i("render prepare failure");
            return;
        }

        // 画面先冲刷一次,防止上次残留
        render->clearColor(0.0, 0.0, 0.0, 1.0); // 黑色
        logger.i("render clear color");
        // 只要设置了 surface 立即渲染一次
        if (lastFrame)
        {
            logger.i("render draw lastFrame");
            render->draw(lastFrame);
        }
    }
}

bool HelloVideoPlayer::onProcess(std::shared_ptr<InputDataCtx> inputData)
{
    // 修改资源操作，需挂锁
    std::unique_lock<std::mutex> locker(mutex);

    auto *native = dynamic_cast<HelloVideoPlayer *>(inputData->userdata);
    std::shared_ptr<IAVFrame> &frame = inputData->data;

    HelloClock *masterClock = &avSync->getMasterClock();
    bool isMasterClock = avSync->isMasterClock(IAVMediaType::VIDEO);
    // 这里需要特殊判断，如果音频数据比视频数据要靠后才会播放时，优先使用视频fps来处理渲染
    /*
     * eg: A MP4 file like this:
     * |- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -|
     * |                            Video Stream Data                              |
     * |- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -|
     *                | - - - - - - - - - - - - - - - - - - - - |
     *                |             Audio Stream Data           |
     *                | - - - - - - - - - - - - - - - - - - - - |
     * |- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -|
     * 0s             2s                                        7s                 10s
     * */

    if (!isMasterClock && !masterClock->available())
    {
        // 上述特殊情况，继续使用视频作为主时钟
        isMasterClock = true;
        masterClock = &avSync->getClockByType(IAVMediaType::VIDEO);
//        logger.i("Force to use video clock");
    }

    // 判断包是否过期
    if (frame->serial != masterClock->serial)
    {
        int64_t serial = masterClock->serial;
        logger.i("serial[%d] != master[%d] drop pts[%d]ms",
                 frame->serial, serial, frame->getPtsUs() / 1000);
//        logger.i("Frame[video] is dropped and keep showing last picture");
        return true;
    }

    if (frame->eof)
    {
        logger.i("receive eof");
        // 回调出去，外部可能会做对应逻辑处理
        native->sendCallback(frame);
        return true;
    }

    // 暂停状态，至少渲染一帧（起始帧）
    if (masterClock->isPaused())
    {
//        logger.i("master clock is paused");
    }

//    logger.i("onProcess frame pts[%d]ms serial[%d]",
//             frame->getPtsUs() / 1000, frame->serial);

    // 主时钟，按照自己的节奏送显，判断当前帧还需要持续多久时间
    // 非主时钟，计算时间差，判断送显时间，达到音视频同步
    int64_t delayUs = isMasterClock ? masterClock->getRemainingUs() : masterClock->getDelayUs(
            frame);
    if (delayUs <= 0)
    {
        // 立即送显
//        int64_t startMs = av_gettime_relative();
        native->draw(frame);
//        int64_t costMs = av_gettime_relative() - startMs;
//        logger.i("native->draw(frame) cost[%d]ms", costMs);
        
        // 如果时主时钟，需要更新时间
        if (isMasterClock)
        {
            // 更新当前时钟，方便下次判断时间差
            int64_t ptsUs = frame->getPtsUs() / masterClock->speed;
            int64_t durationUs = frame->getDurationUs() / masterClock->speed;
            masterClock->setPtsUs(ptsUs, durationUs);
        }
//        logger.i("AVPlayer[video] pts[%d]ms duration[%d]ms", frame->getPtsUs() / 1000, frame->getDurationUs() / 1000);
        // 数据回调外部消费者
        sendCallback(frame);
        return true;
    } else
    {
//        logger.i("frame pts[%d]ms delay [%d]ms", frame->getPtsUs() / 1000, delayUs / 1000);
        // 下次轮询判断画面是否需要渲染
        callMeLater(HELLO_VIDEO_REFRESH_US);
        return false;
    }
}

//void save_yuv420p(AVFrame *frame, FILE *file) {
//    
//    int width = frame->width;
//    int height = frame->height;
//
//    // 写入Y平面数据
//    for (int i = 0; i < height; i++) {
//        fwrite(frame->data[0] + i * frame->linesize[0], 1, width, file);
//    }
//    // 写入U平面数据
//    for (int i = 0; i < height / 2; i++) {
//        fwrite(frame->data[1] + i * frame->linesize[1], 1, width / 2, file);
//    }
//    // 写入V平面数据
//    for (int i = 0; i < height / 2; i++) {
//        fwrite(frame->data[2] + i * frame->linesize[2], 1, width / 2, file);
//    }
//
//}

//int64_t lastDrawTimestampsMs = 0;
void HelloVideoPlayer::draw(std::shared_ptr<IAVFrame> &frame)
{
    if (render && render->isPrepared() && prepared)
    {
        // 软件转换像素格式
        AVPixelFormat format = render->getAVPixelFormat();
        AVPixelFormat _format = (AVPixelFormat)frame->frame->format;
        if (_format != format)
        {
            int64_t startMs = av_gettime_relative();
            AVFrame *dst;
            if (videoConverter->convert(frame->frame, format, &dst))
            {
                frame->move(dst);
                av_frame_free(&dst);
            }
            int64_t costMs = av_gettime_relative() - startMs;
            logger.i("videoConverter->convert cost[%d]ms",costMs);
        }

        std::shared_ptr<HelloVideoFrame> image = std::make_shared<AVFrameTexImage>(frame);
        
        // 测试代码
//        if(yuvFile)
//        {
//            save_yuv420p(frame->frame, yuvFile);
//        }
        
//        int64_t startMs = TimeUtil::getCurrentTimeMs();
        render->draw(image);
//        int64_t costMs = TimeUtil::getCurrentTimeMs() - startMs;
//        int64_t currMs = TimeUtil::getCurrentTimeMs();
//        if (lastDrawTimestampsMs >= 0)
//        {
//            int64_t intervalMs = currMs - lastDrawTimestampsMs;
//            float fps = 1000.0 / intervalMs;
//            logger.i("native->draw(frame) cost[%d]ms fps[%.0f] interval[%d]ms", costMs, fps, intervalMs);
//        }
//        lastDrawTimestampsMs = currMs;
//        logger.i("native->draw(frame) cost[%d]ms", costMs);
        lastFrame = image;
    }
}


void HelloVideoPlayer::onReset()
{
    prepared = false;

    // 修改资源操作，需挂锁
    std::unique_lock<std::mutex> locker(mutex);

    waitForAdd.clear();
    waitForRemove.clear();
    lastFrame = nullptr;
}
