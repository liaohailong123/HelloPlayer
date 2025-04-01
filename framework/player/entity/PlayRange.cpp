//
// Created by 廖海龙 on 2025/1/10.
//

#include "PlayRange.hpp"


PlayRange::PlayRange() : startUs(-1), endUs(-1), time_base(), eof(false), seekOnce(false)
{
    // 默认时间基：us
    time_base = {1, 1000000};
}

PlayRange::~PlayRange() = default;

int64_t PlayRange::getDuration(AVRational *timebase) const
{
    int64_t length = endUs - startUs;
    if (timebase)
    {
        return av_rescale_q(length, time_base, *timebase);
    }
    return length;
}

int64_t PlayRange::getEndPts(AVRational *timebase) const
{
    if (timebase)
    {
        return av_rescale_q(endUs, time_base, *timebase);
    }
    return endUs;
}

/**
 * @param other 把外部数据拷贝过来
 */
void PlayRange::copy(const PlayRange &other)
{
    this->startUs = other.startUs;
    this->endUs = other.endUs;
    this->time_base = other.time_base;
    this->serial = other.serial;
    this->eof = other.eof;
}

void PlayRange::seekToPtsUs(int64_t ptsUs)
{
    startUs = endUs =  ptsUs;
    eof = false;
}