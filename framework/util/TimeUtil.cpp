//
// Created by 廖海龙 on 2024/11/18.
//

#include "TimeUtil.hpp"


int64_t TimeUtil::getCurrentTimeMs()
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return ms.count();
}

int64_t TimeUtil::getCurrentTimeUs()
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    return ms.count();
}