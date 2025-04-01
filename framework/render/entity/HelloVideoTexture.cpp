//
// Created by liaohailong on 2025/2/27.
//

#include "HelloVideoTexture.hpp"

#include <utility>

HelloVideoTexture::HelloVideoTexture(std::shared_ptr<HelloVideoFrame> frame) :
        logger("HelloVideoTexture"), frame(std::move(frame))
{
//    logger.i("HelloVideoTexture::HelloVideoTexture(%p)", this);
}

HelloVideoTexture::~HelloVideoTexture()
{
//    logger.i("HelloVideoTexture::~HelloVideoTexture(%p)", this);
}

uint8_t **HelloVideoTexture::getData()
{
    return frame->getData();
}

int *HelloVideoTexture::getLineSize()
{
    return frame->getLineSize();
}

int HelloVideoTexture::getFormat()
{
    return frame->getFormat();
}

int HelloVideoTexture::getWidth()
{
    return frame->getWidth();
}

int HelloVideoTexture::getHeight()
{
    return frame->getHeight();
}