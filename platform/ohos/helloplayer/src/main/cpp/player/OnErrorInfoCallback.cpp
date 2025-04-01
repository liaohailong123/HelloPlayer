//
// Created on 2025/3/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "OnErrorInfoCallback.hpp"

OnErrorInfoCallback::OnErrorInfoCallback(napi_env env, napi_value this_arg, napi_value value)
    : NapiCallback(env, this_arg, value, "OnErrorInfoCallback") 
{
    logger.i("OnErrorInfoCallback::OnErrorInfoCallback(%p)", this);
}
OnErrorInfoCallback::~OnErrorInfoCallback() 
{
    logger.i("OnErrorInfoCallback::~OnErrorInfoCallback(%p)", this);
}

void OnErrorInfoCallback::onNapiCallbackData(napi_env env, napi_value recv, napi_value js_cb, NapiCallbackData *data) 
{
    size_t argc = 2;
    napi_value argv[2];

    napi_create_int64(env, data->long0, &argv[0]);                        // code: number
    napi_create_string_utf8(env, data->str0, NAPI_AUTO_LENGTH, &argv[1]); // msg: string
    
    napi_call_function(env, recv, js_cb, argc, argv, nullptr);
}
