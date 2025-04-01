//
// Created by liaohailong on 2025/1/12.
//

#ifndef HELLOPLAYER_HELLOVIDEOPLAYER_HPP
#define HELLOPLAYER_HELLOVIDEOPLAYER_HPP

#include <utility>

#include "HelloProcessor.hpp"

#include "../entity/IAVFrame.hpp"
#include "../entity/AVProperties.hpp"
#include "../HelloAVSync.hpp"
#include "../entity/PlayConfig.hpp"
#include "../../util/VideoConverter.hpp"
#include "../../util/TimeUtil.hpp"

#include "../device/HelloVideoRender.hpp"
#include "../device/HelloDeviceRegister.hpp"


// 视频刷新间隔，60帧/1秒=16.7ms，这里开3倍速的话，5ms是够的了，
// 如果上层倍速过大，可能不好同步了，另外还需要考虑解码速度跟不上播放速度的问题
//#define HELLO_VIDEO_REFRESH_US (5 * 1000)
#define HELLO_VIDEO_REFRESH_US (1000)


/**
 * create by liaohailong
 * 2025/1/12 11:41
 * desc: 
 */
class HelloVideoPlayer : public HelloProcessor<IAVFrame, IAVFrame>
{
    /**
     * 适配 SimpleCanvas 输入的数据结构
     */
    class AVFrameTexImage : public HelloVideoFrame
    {
    public:
        explicit AVFrameTexImage(std::shared_ptr<IAVFrame> frame) : HelloVideoFrame("HelloAVFrame")
        {
            this->frame = std::move(frame);
//            logger.i("AVFrameTexImage()");
        }

        ~AVFrameTexImage() override
        {
//            logger.i("~AVFrameTexImage()");
        }

        uint8_t **getData() override
        {
            return frame->frame->data;
        }

        int *getLineSize() override
        {
            return frame->frame->linesize;
        }

        int getFormat() override
        {
            return frame->frame->format;
        }

        int getWidth() override
        {
            return frame->frame->width;
        }

        int getHeight() override
        {
            return frame->frame->height;
        }

    private:
        std::shared_ptr<IAVFrame> frame;
    };

public:
    explicit HelloVideoPlayer(std::shared_ptr<HelloAVSync> avSync);

    ~HelloVideoPlayer() override;

    void addSurface(void *surface);

    void removeSurface(void *surface);

    void prepare(const std::shared_ptr<VideoProperties> &properties, PlayConfig *config);

    inline bool isPrepared() const noexcept
    {
        return prepared;
    }

protected:
    bool onProcess(std::shared_ptr<InputDataCtx> inputData) override;
    void onReset() override;

private:
    static void initRender(void *userdata);

    void addWaitingSurface();

    void draw(std::shared_ptr<IAVFrame> &frame);

private:
    /**
     * 是否已经准备好
     */
    std::atomic<bool> prepared;
    /**
     * 外部线程与内部handler操作相应需要同步
     */
    std::mutex mutex;

    /**
     * 视频基本信息
     */
    std::shared_ptr<VideoProperties> properties;

    /**
     * 绘制 YUV420 或 YUV422 格式画面
     */
    std::shared_ptr<HelloVideoRender> render;

    /**
     * 上一帧绘制的画面保留
     */
    std::shared_ptr<HelloVideoFrame> lastFrame;

    /**
     * 待添加的 surface
     */
    std::vector<void *> waitForAdd;
    /**
     * 待移除的 surface
     */
    std::vector<void *> waitForRemove;

    /**
     * 同步器
     */
    std::shared_ptr<HelloAVSync> avSync;


    /**
     * 视频数据格式转换
     */
    std::shared_ptr<VideoConverter> videoConverter;
};


#endif //HELLOPLAYER_HELLOVIDEOPLAYER_HPP
