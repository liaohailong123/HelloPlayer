//
// Created on 2025/3/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "OnPreparedCallback.hpp"

OnPreparedCallback::OnPreparedCallback(napi_env env, napi_value this_arg, napi_value value)
    : NapiCallback(env, this_arg, value, "OnPreparedCallback") 
{
    logger.i("OnPreparedCallback::OnPreparedCallback(%p)", this);
}
OnPreparedCallback::~OnPreparedCallback() 
{
    logger.i("OnPreparedCallback::~OnPreparedCallback(%p)", this);
}

void OnPreparedCallback::onNapiCallbackData(napi_env env, napi_value recv, napi_value js_cb, NapiCallbackData *data) 
{
    size_t argc = 1;
    napi_value info; // info: HelloPlayerMediaInfo

    napi_create_object(env, &info);
    
    napi_value url;
    napi_value startOffsetUs;
    napi_value audioDurationUs;
    napi_value videoDurationUs;
    napi_value sampleRate;
    napi_value channelCount;
    napi_value sampleFmt;
    napi_value width;
    napi_value height;
    napi_value frameRate;
    napi_value masterClockType;
    napi_value isLiveStreaming;
    
    napi_create_string_utf8(env, data->str0, NAPI_AUTO_LENGTH, &url);
    napi_create_int64(env, data->long0, &startOffsetUs);
    napi_create_int64(env, data->long1, &audioDurationUs);
    napi_create_int64(env, data->long2, &videoDurationUs);
    napi_create_int64(env, data->long3, &sampleRate);
    napi_create_int64(env, data->long4, &channelCount);
    napi_create_int64(env, data->long5, &sampleFmt);
    napi_create_int64(env, data->long6, &width);
    napi_create_int64(env, data->long7, &height);
    napi_create_int64(env, data->long8, &frameRate);
    napi_create_int64(env, data->long9, &masterClockType);
    napi_get_boolean(env, data->bool0, &isLiveStreaming);

    
    napi_set_named_property(env, info, "url", url);
    napi_set_named_property(env, info, "startOffsetUs", startOffsetUs);
    napi_set_named_property(env, info, "audioDurationUs", audioDurationUs);
    napi_set_named_property(env, info, "videoDurationUs", videoDurationUs);
    napi_set_named_property(env, info, "sampleRate", sampleRate);
    napi_set_named_property(env, info, "channelCount", channelCount);
    napi_set_named_property(env, info, "sampleFmt", sampleFmt);
    napi_set_named_property(env, info, "width", width);
    napi_set_named_property(env, info, "height", height);
    napi_set_named_property(env, info, "frameRate", frameRate);
    napi_set_named_property(env, info, "masterClockType", masterClockType);
    napi_set_named_property(env, info, "isLiveStreaming", isLiveStreaming);
    
    
    napi_call_function(env, recv, js_cb, argc, &info, nullptr);
}
