//
// Created on 2025/3/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OHOS_NAPICALLBACK_H
#define OHOS_NAPICALLBACK_H

#include "napi/native_api.h"

#include "framework/log/Logger.hpp"
#include "NapiCallbackData.hpp"


class NapiCallback {
public:
    explicit NapiCallback(napi_env env, napi_value this_arg, napi_value value, const char *work_name);
    virtual ~NapiCallback();

    void call(NapiCallbackData *data = nullptr);

private:
    static void call_js_cb(napi_env env, napi_value js_cb, void *context, void *data);

protected:
    virtual void onNapiCallbackData(napi_env env, napi_value recv, napi_value js_cb, NapiCallbackData *data) = 0;

protected:
    Logger logger;

private:
    napi_threadsafe_function tsfn;
    napi_ref recvObj;
    napi_ref cbObj;
};

#endif // OHOS_NAPICALLBACK_H
