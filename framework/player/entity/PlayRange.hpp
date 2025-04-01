//
// Created by 廖海龙 on 2025/1/10.
//

#ifndef HELLOPLAYER_PLAYRANGE_HPP
#define HELLOPLAYER_PLAYRANGE_HPP

extern "C" {
#include <libavformat/avformat.h>
}

/**
 * Author: liaohailong
 * Date: 2025/1/10
 * Time: 14:30
 * Description: 播放范围
 **/
class PlayRange
{
public:
    PlayRange();

    ~PlayRange();

    /**
     * @param timebase 外部可指定时间基，不传就使用内部 time_base(单位：us)
     * @return 获取播放片段时间
     */
    int64_t getDuration(AVRational *timebase = nullptr) const;

    /**
     * @param timebase 外部可指定时间基，不传就使用内部 time_base(单位：us)
     * @return 获取结束时间
     */
    int64_t getEndPts(AVRational *timebase = nullptr) const;

    /**
     * @param other 把外部数据拷贝过来
     */
    void copy(const PlayRange &other);

    void seekToPtsUs(int64_t ptsUs);

public:
    int64_t startUs;
    int64_t endUs;
    int64_t serial;
    bool eof;
    bool seekOnce;

private:
    AVRational time_base; // 默认单位：us
};


#endif //HELLOPLAYER_PLAYRANGE_HPP
