//
// Created by liaohailong on 2025/1/11.
//

#ifndef HELLOPLAYER_HELLOAUDIOPLAYER_HPP
#define HELLOPLAYER_HELLOAUDIOPLAYER_HPP

#include "HelloProcessor.hpp"

#include "../entity/IAVFrame.hpp"
#include "../entity/AVProperties.hpp"
#include "../entity/PlayConfig.hpp"
#include "../device/HelloAudioRender.hpp"
#include "../device/HelloDeviceRegister.hpp"
#include "../../util/AudioController.hpp"
#include "../HelloAVSync.hpp"

extern "C" {
#include <libavutil/channel_layout.h>
#include <libavutil/fifo.h>
}


/**
 * create by liaohailong
 * 2025/1/11 20:50
 * desc: 
 */
class HelloAudioPlayer : public HelloProcessor<IAVFrame, IAVFrame>
{
    typedef struct
    {
        int64_t samples;
        std::shared_ptr<IAVFrame> frame;
    } FillDataResult;
public:
    explicit HelloAudioPlayer(std::shared_ptr<HelloAVSync> avSync);

    ~HelloAudioPlayer() override;

    void prepare(const std::shared_ptr<AudioProperties> &properties, PlayConfig *config);

    inline bool isPrepared() const noexcept
    {
        return prepared;
    }

    /**
     * @param volume 0.0=静音 1.0=原始音量 大于1.0=扩大音量
     */
    void setVolume(double volume);

    bool isDefaultVolume() const;

    /**
     * @param speed  音频速率，默认1.0 范围[0.5,100]
     */
    void setSpeed(double speed);

    bool isDefaultSpeed() const;

    /**
     * @return true表示需要重采样
     */
    bool needResample();

protected:
    bool onProcess(std::shared_ptr<InputDataCtx> inputData) override;
    void onReset() override;

private:

    /**
     * 按需初始化音频处理器
     * @param frame 一帧音频数据，里面携带采样率、声道数、位深类型
     */
    void ensureAudioControllerIfNecessary(const std::shared_ptr<IAVFrame>& frame);

    void releaseAudioController();

    void closeAudioRender();

    static void onDataCallback(HelloAudioRender *render,
                               void *audioData, int32_t numFrames, void* userdata);

    static std::shared_ptr<FillDataResult> onFillData(const std::shared_ptr<IAVFrame>& frame,
                           void *audioData, int64_t writeOffset,
                           int32_t numFrames, void* userdata);


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
     * 音频配置
     */
    std::shared_ptr<AudioProperties> properties;

    /**
     * 音频渲染器返回的音频配置
     */
    std::shared_ptr<AudioProperties> renderProperties;

    /**
     * 音频播放器
     */
     std::shared_ptr<HelloAudioRender> render;

    /**
     * 同步器
     */
    std::shared_ptr<HelloAVSync> avSync;

    /**
     * 音频大小，默认1.0; 0.0=静音 1.0=原始音量 大于1.0=扩大音量
     */
    double volume;

    /**
     * 音频速率，默认1.0 范围[0.5,100]
     */
    double speed;

    /**
     * 音频控制器：音量大小，音频变速
     */
    std::shared_ptr<AudioController> audioController;

    /**
     * 标记位：音频控制器需要改变
     */
    bool audioControllerChanged;

    /**
     * 用来接收算法处理后的音频数据
     */
    std::shared_ptr<IAVFrame> lastFrame;


};


#endif //HELLOPLAYER_HELLOAUDIOPLAYER_HPP
