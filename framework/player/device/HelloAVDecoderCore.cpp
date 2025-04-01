//
// Created by liaohailong on 2025/3/15.
//

#include "HelloAVDecoderCore.hpp"

HelloAVDecoderCore::HelloAVDecoderCore(const char *tag) : logger(tag), stream(nullptr),
                                                          config(), callback(nullptr),
                                                          userdata(nullptr)
{
    logger.i("HelloAVDecoderCore::HelloAVDecoderCore(%p)", this);
}

HelloAVDecoderCore::~HelloAVDecoderCore()
{
    this->callback = nullptr;
    this->userdata = nullptr;
    this->stream = nullptr;
    logger.i("HelloAVDecoderCore::~HelloAVDecoderCore(%p)", this);
}

bool HelloAVDecoderCore::needAnnexB()
{
    return false;
}

/**
 * @param _callback 注册 解码帧完毕 回调
 * @param _userdata 外部数据
 */
void HelloAVDecoderCore::setReceiveFrameCallback(OnReceiveFrameCallback _callback, void *_userdata)
{
    this->callback = _callback;
    this->userdata = _userdata;
}