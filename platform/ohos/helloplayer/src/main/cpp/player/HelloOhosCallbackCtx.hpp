//
// Created on 2025/3/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OHOS_HELLOOHOSCALLBACKCTX_H
#define OHOS_HELLOOHOSCALLBACKCTX_H

#include "OnBufferPositionCallback.hpp"
#include "OnCurrentPositionCallback.hpp"
#include "OnErrorInfoCallback.hpp"
#include "OnPlayStateChangedCallback.hpp"
#include "OnPreparedCallback.hpp"

typedef struct {
    OnBufferPositionCallback *onBufferPositionCallback;
    OnCurrentPositionCallback *onCurrentPositionCallback;
    OnErrorInfoCallback *onErrorInfoCallback;
    OnPlayStateChangedCallback *onPlayStateChangedCallback;
    OnPreparedCallback *onPreparedCallback;
} HelloOhosCallbackCtx;

#endif //OHOS_HELLOOHOSCALLBACKCTX_H
