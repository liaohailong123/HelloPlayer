//
// Created on 2025/3/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "NapiCallback.hpp"

NapiCallback::NapiCallback(napi_env env, napi_value this_arg, napi_value value, const char *name):logger("NapiCallback")
{
    logger.i("NapiCallback::NapiCallback(%s,%p)", name, this);

    napi_value work_name;
    napi_status status = napi_create_reference(env, this_arg, 1, &recvObj);
    logger.i("napi_create_reference(recvObj) status is %{public}d", status);
    status = napi_create_reference(env, value, 1, &cbObj);
    logger.i("napi_create_reference(cbObj) status is %{public}d", status);
    napi_create_string_utf8(env, name, NAPI_AUTO_LENGTH, &work_name);
    napi_create_threadsafe_function(env, value, NULL, work_name, 0, 1, NULL, NULL, this, NapiCallback::call_js_cb,
                                    &tsfn);
}
NapiCallback::~NapiCallback()
{
    if(tsfn)
    {
        napi_release_threadsafe_function(tsfn, napi_tsfn_release);
        tsfn = nullptr;
    }
    
    logger.i("NapiCallback::~NapiCallback(%p)",this);
}

void NapiCallback::call(NapiCallbackData *data)
{
    if (tsfn != nullptr) {
        napi_call_threadsafe_function(tsfn, data, napi_tsfn_blocking);
    } else {
        logger.i("NapiCallback::call(%p) tsfn is null", this);
    }
}

void NapiCallback::call_js_cb(napi_env env, napi_value js_cb, void *context, void *data)
{
    NapiCallback* napiCallback = reinterpret_cast<NapiCallback*>(context);
    Logger &logger = napiCallback->logger;
    logger.i("NapiCallback::call_js_cb(%p)", napiCallback);

    if (env == nullptr) {
        logger.i("napiCallback env is null");
        return;
    }
    if (napiCallback == nullptr) {
        logger.i("napiCallback napiCallback is null");
        return;
    }
    if (napiCallback->cbObj == nullptr) {
        logger.i("napiCallback cbObj is null");
        return;
    }
    
    napi_value recv2 = nullptr;
    napi_status status = napi_get_reference_value(env, napiCallback->recvObj, &recv2);
    if (napi_ok != status || !recv2) {
        logger.i("napiCallback recv2 is null");
        return;
    }

    napi_value js_cb2 = nullptr;
    status = napi_get_reference_value(env, napiCallback->cbObj, &js_cb2);
    if (napi_ok != status || !js_cb2) {
        logger.i("napiCallback js_cb2 is null");
        return;
    }
    
    NapiCallbackData *napiCallbackData = (NapiCallbackData *)data;
    napiCallback->onNapiCallbackData(env, recv2, js_cb2, napiCallbackData);
    delete napiCallbackData;
}
