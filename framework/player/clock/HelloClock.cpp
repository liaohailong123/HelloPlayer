//
// Created by liaohailong on 2025/1/12.
//

#include "HelloClock.hpp"

HelloClock::HelloClock(const char *tag) : logger(tag), name(tag), mutex(),
                                          ptsUs(-1), ptsDriftUs(-1), durationUs(-1), speed(1.0),
                                          paused(false), lastUpdateUs(0), serial(0),
                                          callback(nullptr), userdata(nullptr)
{
    logger.i("HelloClock::HelloClock(%p)", this);
}

HelloClock::~HelloClock()
{
    logger.i("HelloClock::~HelloClock(%p)", this);
}

std::string &HelloClock::getName()
{
    return name;
}

/**
 * @param pts 设置当前时间，单位：us
 */
void HelloClock::setPtsUs(int64_t pts, int64_t _durationUs)
{
    setPtsUsAt(pts, _durationUs, av_gettime_relative());
}

void HelloClock::setPtsUsAt(int64_t pts, int64_t _durationUs, int64_t timestampsUs)
{
    // 挂锁，维护数据安全
    std::unique_lock<std::mutex> locker(mutex);

    lastUpdateUs = timestampsUs;
    ptsUs = pts; // 变速后的值
    ptsDriftUs = ptsUs - lastUpdateUs;
    durationUs = _durationUs; // 变速后相对于系统时间的值

    // 提前放锁
    locker.unlock();

    // 同步外部更新
    if (callback)
    {
        int64_t currentPtsUs = getCurrentPtsUs();
        double _speed = speed;
        int64_t adjustPtsUs = currentPtsUs * _speed; // 返回外部的时间需要还原倍速
        callback(adjustPtsUs, userdata);
    }
}

bool HelloClock::available() const
{
    return ptsUs >= 0;
}

/**
 * @return 获取当前播放时间
 */
int64_t HelloClock::getCurrentPtsUs()
{

    if (!available())return 0; // 尚未设置过时钟，返回0即可

    if (paused)
    {
        return ptsUs;
    }
    int64_t time = av_gettime_relative();
    // pts变速后的值变小，+号后面的数学式子是把时间还原回来
    return ptsDriftUs + time - (time - lastUpdateUs) * (1.0 - speed);
}

/**
 * @return 获取当前帧，还需要持续多久时间，单位：us
 */
int64_t HelloClock::getRemainingUs()
{
    // 挂锁，维护数据安全
    std::unique_lock<std::mutex> locker(mutex);

    if (!available()) return 0; // 尚未设置过时钟，返回0即可

    int64_t endPtsUs = ptsUs + durationUs; // 变速后相对于系统时间的值
    int64_t currentPtsUs = getCurrentPtsUs();
    int64_t remainingUs = endPtsUs - currentPtsUs;
//    logger.i("endPtsUs[%d] currentPtsUs[%d] remainingUs[%d]", endPtsUs, currentPtsUs, remainingUs);
    return FFMAX(remainingUs, 0);
}

/**
 * @param frame 计算该frame落后主时钟的时间
 * @return 小于0表示该帧时间落后于时钟，大于0表示该帧是未来才需要播放的
 */
int64_t HelloClock::getDelayUs(const std::shared_ptr<IAVFrame> &frame)
{
    // 挂锁，维护数据安全
    std::unique_lock<std::mutex> locker(mutex);

    // 当前播放到哪个时间
    int64_t currentPtsUs = getCurrentPtsUs();

    int64_t targetPtsUs = frame->getPtsUs() / speed;
    int64_t diff = targetPtsUs - currentPtsUs;

    return diff;
}

void HelloClock::setCallback(Callback _callback, void *_userdata)
{
    this->callback = _callback;
    this->userdata = _userdata;
}

/**
 * @param _paused 设置时钟是否暂停
 */
void HelloClock::setPaused(bool _paused)
{
    this->paused = _paused;
}

bool HelloClock::isPaused()
{
    return this->paused;
}

/**
 * @return 是否为正常播放速度
 */
bool HelloClock::isDefaultSpeed()
{
    return speed == 1.0;
}

void HelloClock::reset(int64_t _serial)
{
    ptsUs = ptsDriftUs = durationUs = -1;
    serial = _serial;
}