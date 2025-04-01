//
// Created by 廖海龙 on 2025/2/28.
//


#include "HelloPlayerCallback.hpp"

HelloPlayerCallback::HelloPlayerCallback(void* userdata) : logger("HelloPlayerCallback")
{
    logger.i("HelloPlayerCallback::HelloPlayerCallback(%p,%p)", userdata, this);
}

HelloPlayerCallback::~HelloPlayerCallback()
{
logger.i("HelloPlayerCallback::~HelloPlayerCallback(%p)",this);
}