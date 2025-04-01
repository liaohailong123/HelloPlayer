//
// Created by liaohailong on 2025/2/27.
//

#include "HelloVideoRender.hpp"

HelloVideoRender::HelloVideoRender(const char *tag, AVPixelFormat format) : logger(tag)
{
    logger.i("HelloVideoRender::HelloVideoRender(%p)", this);
}

HelloVideoRender::~HelloVideoRender()
{
    logger.i("HelloVideoRender::~HelloVideoRender(%p)", this);
}