//
// Created on 2025/3/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "OnPlayStateChangedCallback.hpp"

OnPlayStateChangedCallback::OnPlayStateChangedCallback(napi_env env, napi_value this_arg, napi_value value)
    : NapiCallback(env, this_arg, value, "OnPlayStateChangedCallback") 
{
    logger.i("OnPlayStateChangedCallback::OnPlayStateChangedCallback(%p)", this);
}
OnPlayStateChangedCallback::~OnPlayStateChangedCallback() 
{
    logger.i("OnPlayStateChangedCallback::~OnPlayStateChangedCallback(%p)", this);
}

void OnPlayStateChangedCallback::onNapiCallbackData(napi_env env, napi_value recv, napi_value js_cb, NapiCallbackData *data) 
{
    size_t argc = 2;
    napi_value argv[2];
    
    napi_create_int64(env, data->long0, &argv[0]); // from: number
    napi_create_int64(env, data->long1, &argv[1]); // to: number
    
    napi_call_function(env, recv, js_cb, argc, argv, nullptr);
}
