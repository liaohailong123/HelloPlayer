//
// Created by liaohailong on 2024/12/22.
//

#ifndef HELLOPLAYER_IAVTIME_HPP
#define HELLOPLAYER_IAVTIME_HPP

#include <cstdint>

/**
 * create by liaohailong
 * 2024/12/22 14:31
 * desc: 接口：提供获取时间的能力
 */
class IAVTime
{
public:
    IAVTime() = default;

    virtual ~IAVTime() = default;

    /**
     * @return 当前数据包的呈现时间，单位：us
     */
    virtual int64_t getPtsUs() = 0;

    /**
     * @return 当前数据包的持续时间，单位：us
     */
    virtual int64_t getDurationUs() = 0;
};


#endif //HELLOPLAYER_IAVTIME_HPP
