//
// Created on 2025/3/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OHOS_ONPREPAREDCALLBACK_H
#define OHOS_ONPREPAREDCALLBACK_H

#include "NapiCallback.hpp"

class OnPreparedCallback : public NapiCallback {
public:
    explicit OnPreparedCallback(napi_env env, napi_value this_arg, napi_value value);
    ~OnPreparedCallback() override;

protected:
    void onNapiCallbackData(napi_env env, napi_value recv, napi_value js_cb, NapiCallbackData *data) override;
};

#endif //OHOS_ONPREPAREDCALLBACK_H
