//
// Created by 廖海龙 on 2025/1/21.
//

#ifndef HELLOPLAYER_VIDEOCONVERTER_HPP
#define HELLOPLAYER_VIDEOCONVERTER_HPP

#include "../log/Logger.hpp"

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/imgutils.h>
#include <libavutil/timestamp.h>
#include <libavutil/time.h>
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
}


/**
 * Author: liaohailong
 * Date: 2025/1/21
 * Time: 16:52
 * Description:
 **/
class VideoConverter {
public:
    explicit VideoConverter();

    ~VideoConverter();

    bool convert(AVFrame *srcFrame, AVPixelFormat dstFormat, AVFrame **dstFrame);

private:
    Logger logger;
    struct SwsContext *swsCtx;
    AVPixelFormat dstFormat;
};


#endif //HELLOPLAYER_VIDEOCONVERTER_HPP
