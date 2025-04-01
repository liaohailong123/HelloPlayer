//
// Created by liaohailong on 2024/6/22.
//

#ifndef HELLOGL_FFUTIL_HPP
#define HELLOGL_FFUTIL_HPP

#include "../log/Logger.hpp"

#include <functional>

extern "C"
{
#include <libavutil/frame.h>
}

/**
 * create by liaohailong
 * 2024/6/22 12:11
 * desc: 
 */
namespace FFUtil {

    /**
     * 判断位深数据是否为planner摆放方式
     * @param fmt 位深格式
     * @return true表示为planner格式
     */
    bool is_planner_fmt(AVSampleFormat &fmt);

    // 打印一下错误信息
    void av_print_error(Logger &logger, int ret, const char *msg);
}


#endif //HELLOGL_FFUTIL_HPP
