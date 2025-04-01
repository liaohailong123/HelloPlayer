//
// Created on 2025/3/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OHOS_ONBUFFERPOSITIONCALLBACK_H
#define OHOS_ONBUFFERPOSITIONCALLBACK_H

#include "NapiCallback.hpp"

class OnBufferPositionCallback : public NapiCallback {
public:
    explicit OnBufferPositionCallback(napi_env env, napi_value this_arg, napi_value value);
    ~OnBufferPositionCallback() override;

protected:
    void onNapiCallbackData(napi_env env, napi_value recv, napi_value js_cb, NapiCallbackData *data) override;
};

#endif //OHOS_ONBUFFERPOSITIONCALLBACK_H
