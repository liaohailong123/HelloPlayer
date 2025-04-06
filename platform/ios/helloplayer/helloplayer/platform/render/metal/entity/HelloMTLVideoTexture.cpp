//
//  HelloMTLVideoTexture.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/4/1.
//

#include "HelloMTLVideoTexture.hpp"


HelloMTLVideoTexture::HelloMTLVideoTexture(std::shared_ptr<HelloVideoFrame> frame) :
        logger("HelloVideoTexture"), frame(std::move(frame))
{
//    logger.i("HelloMTLVideoTexture::HelloMTLVideoTexture(%p)", this);
}

HelloMTLVideoTexture::~HelloMTLVideoTexture()
{
    frame = nullptr;
//    logger.i("HelloMTLVideoTexture::~HelloMTLVideoTexture(%p)", this);
}

uint8_t **HelloMTLVideoTexture::getData()
{
    return frame->getData();
}

int *HelloMTLVideoTexture::getLineSize()
{
    return frame->getLineSize();
}

int HelloMTLVideoTexture::getFormat()
{
    return frame->getFormat();
}

int HelloMTLVideoTexture::getWidth()
{
    return frame->getWidth();
}

int HelloMTLVideoTexture::getHeight()
{
    return frame->getHeight();
}
