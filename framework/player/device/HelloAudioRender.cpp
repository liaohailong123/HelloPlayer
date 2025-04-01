//
// Created by 廖海龙 on 2025/2/28.
//

#include "HelloAudioRender.hpp"

HelloAudioRender::HelloAudioRender(const char *tag) : logger(tag)
{
    logger.i("HelloAudioRender::HelloAudioRender(%p)", this);
}

HelloAudioRender::~HelloAudioRender() {
    logger.i("HelloAudioRender::~HelloAudioRender(%p)", this);
}

int64_t HelloAudioRender::getDelayFrameCount()
{
    return 0;
}