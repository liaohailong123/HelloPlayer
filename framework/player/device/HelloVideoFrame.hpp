//
// Created by liaohailong on 2025/2/27.
//

#ifndef HELLOPLAYER_HELLOVIDEOFRAME_HPP
#define HELLOPLAYER_HELLOVIDEOFRAME_HPP

#include <cstdio>
#include <memory>

#include "../../log/Logger.hpp"

/**
 * create by liaohailong
 * 2025/2/27 20:23
 * desc: HelloVideoRender所消耗的绘制画面内容
 */
class HelloVideoFrame
{
public:
    explicit HelloVideoFrame(const char *tag) : logger(tag)
    {
//        logger.i("HelloVideoFrame::HelloVideoFrame(%p)", this);
    }

    virtual ~HelloVideoFrame()
    {
//        logger.i("HelloVideoFrame::~HelloVideoFrame(%p)", this);
    }

    virtual uint8_t **getData() = 0;

    virtual int *getLineSize() = 0;

    virtual int getFormat() = 0;

    virtual int getWidth() = 0;

    virtual int getHeight() = 0;

protected:
    Logger logger;
};


#endif //HELLOPLAYER_HELLOVIDEOFRAME_HPP
