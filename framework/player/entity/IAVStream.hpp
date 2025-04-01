//
// Created by liaohailong on 2024/12/19.
//

#ifndef HELLOPLAYER_IAVSTREAM_HPP
#define HELLOPLAYER_IAVSTREAM_HPP

extern "C"
{
#include <libavformat/avformat.h>
}


#include "../../log/Logger.hpp"


#include "IAVMediaType.hpp"
#include "AVProperties.hpp"


/**
 * create by liaohailong
 * 2024/12/19 22:12
 * desc: 轨道信息，统一规范
 */
class IAVStream
{
public:
    explicit IAVStream(const char *tag = "IAVStream");

    virtual ~IAVStream();

public:
    Logger logger;
    /**
     * 轨道下标
     */
    int index;
    /**
     * 轨道数据类型
     */
    IAVMediaType type;

    /**
     * 音频参数信息
     */
    std::shared_ptr<AudioProperties> audioProperties;

    /**
     * 视频参数信息
     */
    std::shared_ptr<VideoProperties> videoProperties;

    /**
     * 轨道的实现对象
     */
    AVStream *stream;
};


#endif //HELLOPLAYER_IAVSTREAM_HPP
