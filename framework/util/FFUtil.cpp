//
// Created by liaohailong on 2024/6/22.
//

#include "FFUtil.hpp"


bool FFUtil::is_planner_fmt(AVSampleFormat &fmt)
{
    bool result = false;
    switch (fmt)
    {
        case AV_SAMPLE_FMT_U8P:
        case AV_SAMPLE_FMT_S16P:
        case AV_SAMPLE_FMT_S32P:
        case AV_SAMPLE_FMT_FLTP:
        case AV_SAMPLE_FMT_DBLP:
        case AV_SAMPLE_FMT_S64P:
            result = true;
            break;

        case AV_SAMPLE_FMT_NONE:
            break;
        case AV_SAMPLE_FMT_U8:
            break;
        case AV_SAMPLE_FMT_S16:
            break;
        case AV_SAMPLE_FMT_S32:
            break;
        case AV_SAMPLE_FMT_FLT:
            break;
        case AV_SAMPLE_FMT_DBL:
            break;
        case AV_SAMPLE_FMT_S64:
            break;
        case AV_SAMPLE_FMT_NB:
            break;
    }

    return result;
}

// 打印一下错误信息
void FFUtil::av_print_error(Logger &logger, int ret, const char *msg)
{
    char err[1024] = {0};
    std::string format = msg;
    if (av_strerror(ret, err, 1024) == 0)
    {
        format += ": (%d)%s";
        logger.i(format.c_str(), ret, err);
    } else
    {
        format += ": %d";
        logger.i(format.c_str(), ret);
    }
}