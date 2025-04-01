//
// Created by liaohailong on 2025/1/12.
//

#ifndef HELLOPLAYER_HELLOCLOCK_HPP
#define HELLOPLAYER_HELLOCLOCK_HPP

#include <cstdint>
#include "../../log/Logger.hpp"
#include "../entity/IAVFrame.hpp"

#include <mutex>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
}

/**
 * create by liaohailong
 * 2025/1/12 16:53
 * desc: 时钟信息
 */
class HelloClock {
public:
    /**
     * 更新时钟回调
     */
    typedef void(*Callback)(int64_t currentPtsUs, void *userdata);

public:
    explicit HelloClock(const char *tag);

    ~HelloClock();

    /**
     * @return 获取时钟名称
     */
    std::string &getName();

    /**
     * @param pts 设置当前时间，单位：us
     * @param durationUs 设置frame持续时间，单位：us
     */
    void setPtsUs(int64_t pts, int64_t durationUs);

    /**
     *
     * @param pts 设置当前时间，单位：us
     * @param durationUs 设置frame持续时间，单位：us
     * @param timestampsUs 设置当前系统时间，单位：us
     */
    void setPtsUsAt(int64_t pts, int64_t durationUs, int64_t timestampsUs);

    /**
     * @return true表示设置过pts，该时钟有效
     */
    bool available() const;

    /**
     * @return 获取当前播放时间
     */
    int64_t getCurrentPtsUs();

    /**
     * @return 获取当前帧，还需要持续多久时间，单位：us
     */
    int64_t getRemainingUs();

    /**
     * @param frame 计算该frame与主轴的相对时间
     * @return 小于0表示该帧时间落后于时钟，大于0表示该帧是未来才需要播放的
     */
    int64_t getDelayUs(const std::shared_ptr<IAVFrame> &frame);

    /**
     * @param callback 设置时钟回调，返回当前更新的时间
     */
    void setCallback(Callback callback, void *userdata);

    /**
     * @param paused 设置时钟是否暂停
     */
    void setPaused(bool paused);

    bool isPaused();

    /**
     * @return 是否为正常播放速度
     */
    bool isDefaultSpeed();

    void reset(int64_t serial = 0);

public:
    Logger logger;

    std::string name;

    std::mutex mutex;

    int64_t ptsUs;

    int64_t ptsDriftUs;

    int64_t durationUs;

    /**
     * 播放速率，默认1.0 范围 [0.5,3.0]
     */
    std::atomic<double> speed;

    /**
     * 上次设置pts时的系统时间
     */
    int64_t lastUpdateUs;

    /**
     * 序列号，判断数据包是否过期
     */
    std::atomic<int64_t> serial;

private:
    Callback callback;
    void *userdata;

    std::atomic<bool> paused;
};


#endif //HELLOPLAYER_HELLOCLOCK_HPP
