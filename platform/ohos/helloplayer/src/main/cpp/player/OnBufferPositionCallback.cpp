//
// Created on 2025/3/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "OnBufferPositionCallback.hpp"


OnBufferPositionCallback::OnBufferPositionCallback(napi_env env, napi_value this_arg, napi_value value)
    : NapiCallback(env, this_arg, value, "OnBufferPositionCallback") 
{
    logger.i("OnBufferPositionCallback::OnBufferPositionCallback(%p)", this);
}
OnBufferPositionCallback::~OnBufferPositionCallback() 
{
    logger.i("OnBufferPositionCallback::~OnBufferPositionCallback(%p)", this);
}

void OnBufferPositionCallback::onNapiCallbackData(napi_env env, napi_value recv, napi_value js_cb, NapiCallbackData *data) 
{
    size_t argc = 3;
    napi_value argv[3];
    
    napi_create_int64(env, data->long0, &argv[0]); // startUs: number
    napi_create_int64(env, data->long1, &argv[1]); // endUs: number
    napi_create_int64(env, data->long2, &argv[2]); // durationUs: number
    
    napi_call_function(env, recv, js_cb, argc, argv, nullptr);
}