//
// Created by liaohailong on 2024/12/19.
//

#include "IAVStream.hpp"

IAVStream::IAVStream(const char *tag) : logger(tag), index(-1),
                                        type(IAVMediaType::UNKNOWN),
                                        audioProperties(), videoProperties(),
                                        stream(nullptr)
{
    logger.i("IAVStream::IAVStream(%p)", this);
}

IAVStream::~IAVStream()
{
    logger.i("IAVStream::~IAVStream(%p)", this);
}