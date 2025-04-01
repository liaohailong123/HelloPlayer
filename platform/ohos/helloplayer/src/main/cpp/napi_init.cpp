#include "napi/native_api.h"
#include "hilog/log.h"

#include "framework/log/Logger.hpp"
#include "framework/handler/NativeHandler.hpp"
#include "framework/player/HelloPlayer.hpp"
#include "player/HelloOhosCallbackCtx.hpp"
#include <cstdint>
#include <native_window/external_window.h>

#define OHOS_LOG_INFO(tag,msg,...) OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, tag, msg, ##__VA_ARGS__);
#define OHOS_LOG_ERROR(tag,msg,...) OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, tag, msg, ##__VA_ARGS__);

/**
 * 实现 Logger 类的打印日志能力
 * @param level 日志等级
 * @param tag 日志的tag标识
 * @param fmt 格式化文本
 * @param ap 格式化文本中的参数列表
 */
void onLogPrint2(Logger::Level level, const char *tag, const char *fmt, va_list ap)
{

    // 鸿蒙log占位符比较特殊，这里转换成字符串再送打印
    char buffer[8196];
    vsprintf(buffer, fmt, ap);
    
    if (Logger::Level::Info == level)
    {
        OHOS_LOG_INFO(tag,"%{public}s", buffer)
    }
    else if(Logger::Level::Error == level)
    {
        OHOS_LOG_ERROR(tag,"%{public}s", buffer)
    }
}

/**
 * 给 NativeHandler 定义log日志打印功能
 * @param tag log的分类tag
 * @param format 格式化文本
 * @param ... 格式化文本中的填入参数，可选
 * @return 0=success
 */
void onLogPrint(const char *tag, const char *fmt, va_list ap) {
    onLogPrint2(Logger::Level::Info, tag, fmt, ap);
}


static napi_value NAPI_Global_nativeInit(napi_env env, napi_callback_info info) {
/*
export const nativeInit: (
  this_arg: HelloPlayer,
  OnPreparedListener: (info: HelloPlayerMediaInfo) => void,
  OnPlayStateChangedListener: (from: number, to: number) => void,
  OnErrorInfoListener: (code: number, msg: string) => void,
  OnCurrentPositionListener: (ptsUs: number, durationUs: number) => void,
  OnBufferPositionListener: (startUs: number, endUs: number, durationUs: number) => void
) => number;
*/
    size_t argc = 6;
    napi_value args[6];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    
    // napi 异步安全函数回调
    HelloOhosCallbackCtx* cbCtx = new HelloOhosCallbackCtx();
    cbCtx->onPreparedCallback = new OnPreparedCallback(env, args[0], args[1]);
    cbCtx->onPlayStateChangedCallback = new OnPlayStateChangedCallback(env, args[0], args[2]);
    cbCtx->onErrorInfoCallback = new OnErrorInfoCallback(env, args[0], args[3]);
    cbCtx->onCurrentPositionCallback = new OnCurrentPositionCallback(env, args[0], args[4]);
    cbCtx->onBufferPositionCallback = new OnBufferPositionCallback(env, args[0], args[5]);
    
    napi_value ret;
    HelloPlayer *native = new HelloPlayer(cbCtx);
    long ptr = reinterpret_cast<int64_t>(native);
    status = napi_create_int64(env, ptr, &ret);
    if (napi_ok == status) {
        Logger("Victor").i("NAPI_Global_nativeInit(%d)", ptr);
    }
    return ret;
}

/**
 * 设置播放配置信息
 * @param env napi环境
 * @param info @{HelloPlayerConfig.ets}
 * @return void
 */
static napi_value NAPI_Global_nativeSetConfig(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok) {
        return nullptr; // return void
    }
    
    napi_value ptr = args[0];
    napi_value config = args[1];
    
    int64_t nativePtr;
    napi_get_value_int64(env, ptr, &nativePtr);
    
    HelloPlayer* native = reinterpret_cast<HelloPlayer*>(nativePtr);
    
    // 取出 @{HelloPlayerConfig.ets} 中的字段
    napi_value bufferDurationUs;
    napi_value bufferLoadMoreFactor;
    napi_value useHardware;
    napi_value autoPlay;
    napi_value loopPlay;
    napi_value volume;
    napi_value speed;
    napi_value ioTimeoutUs;

    napi_get_named_property(env, config, "bufferDurationUs", &bufferDurationUs);
    napi_get_named_property(env, config, "bufferLoadMoreFactor", &bufferLoadMoreFactor);
    napi_get_named_property(env, config, "useHardware", &useHardware);
    napi_get_named_property(env, config, "autoPlay", &autoPlay);
    napi_get_named_property(env, config, "loopPlay", &loopPlay);
    napi_get_named_property(env, config, "volume", &volume);
    napi_get_named_property(env, config, "speed", &speed);
    napi_get_named_property(env, config, "ioTimeoutUs", &ioTimeoutUs);
    
    PlayConfig _config;

    napi_get_value_int64(env, bufferDurationUs, &_config.bufferDurationUs);
    napi_get_value_double(env, bufferLoadMoreFactor, &_config.bufferLoadMoreFactor);
    napi_get_value_bool(env, useHardware, &_config.useHardware);
    napi_get_value_bool(env, autoPlay, &_config.autoPlay);
    napi_get_value_bool(env, loopPlay, &_config.loopPlay);
    napi_get_value_double(env, volume, &_config.volume);
    napi_get_value_double(env, speed, &_config.speed);
    napi_get_value_int64(env, ioTimeoutUs, &_config.ioTimeoutUs);
    
    native->setConfig(_config);

    return nullptr; // return void
}
/**
 * 参考：https://gitee.com/harmonyos_samples/ComplexTypePass/blob/master/entry/src/main/cpp/napi_init.cpp
 * 双向传递组合类型对象： ts -> napi napi -> ts
 *
 * @param env env napi环境
 * @param info
 * @return @{HelloPlayerConfig.ets}
 */
static napi_value NAPI_Global_nativeGetConfig(napi_env env, napi_callback_info info) {
    
    size_t argc = 1;
    napi_value args[1];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok) {
        return nullptr; // return void
    }
    
    napi_value ptr = args[0];
    
    int64_t nativePtr;
    napi_get_value_int64(env, ptr, &nativePtr);
    
    HelloPlayer* native = reinterpret_cast<HelloPlayer*>(nativePtr);
    
    PlayConfig _config = native->getConfig();
    
    napi_value config;

    napi_create_object(env, &config);
    
    napi_value bufferDurationUs;
    napi_value bufferLoadMoreFactor;
    napi_value useHardware;
    napi_value autoPlay;
    napi_value loopPlay;
    napi_value volume;
    napi_value speed;
    
    napi_create_int64(env, _config.bufferDurationUs , &bufferDurationUs);
    napi_create_double(env, _config.bufferLoadMoreFactor, &bufferLoadMoreFactor);
    napi_get_boolean(env, _config.useHardware, &useHardware);
    napi_get_boolean(env, _config.autoPlay, &autoPlay);
    napi_get_boolean(env, _config.loopPlay, &loopPlay);
    napi_create_double(env, _config.volume, &volume);
    napi_create_double(env, _config.speed, &speed);
    
    napi_set_named_property(env, config, "bufferDurationUs", bufferDurationUs);
    napi_set_named_property(env, config, "bufferLoadMoreFactor", bufferLoadMoreFactor);
    napi_set_named_property(env, config, "useHardware", useHardware);
    napi_set_named_property(env, config, "autoPlay", autoPlay);
    napi_set_named_property(env, config, "loopPlay", loopPlay);
    napi_set_named_property(env, config, "volume", volume);
    napi_set_named_property(env, config, "speed", speed);
    
    return config;
}

static napi_value NAPI_Global_nativeSetDataSource(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status == napi_ok) {

        int64_t ptr;
        napi_get_value_int64(env, args[0], &ptr);

        HelloPlayer *native = reinterpret_cast<HelloPlayer *>(ptr);

        size_t length = 0;
        napi_get_value_string_utf8(env, args[1], nullptr, 0, &length);
        char *url = new char [length + 1];
        napi_get_value_string_utf8(env, args[1], url, length + 1, &length);
        
        native->setDataSource(url);
        
        delete [] url;
    }

    return nullptr; // return void
}

static napi_value NAPI_Global_nativeAddSurface(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status == napi_ok) {

        int64_t ptr;
        napi_get_value_int64(env, args[0], &ptr);

        HelloPlayer *native = reinterpret_cast<HelloPlayer *>(ptr);

        size_t length = 0;
        napi_get_value_string_utf8(env, args[1], nullptr, 0, &length);
        
        char *surfaceId = new char [length + 1];
        napi_get_value_string_utf8(env, args[1], surfaceId, length + 1, &length);
        
        uint64_t sid = std::strtoull(surfaceId, nullptr, 0);
        OHNativeWindow *window = nullptr;
        int ret = OH_NativeWindow_CreateNativeWindowFromSurfaceId(sid, &window);
        if (ret == 0 && window) {
            native->addSurface(window);
        }
        
        delete [] surfaceId;
    }
    return nullptr; // return void
}
static napi_value NAPI_Global_nativeRemoveSurface(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status == napi_ok) {

        int64_t ptr;
        napi_get_value_int64(env, args[0], &ptr);

        HelloPlayer *native = reinterpret_cast<HelloPlayer *>(ptr);

        size_t length = 0;
        napi_get_value_string_utf8(env, args[1], nullptr, 0, &length);
        
        char *surfaceId = new char [length + 1];
        napi_get_value_string_utf8(env, args[1], surfaceId, length + 1, &length);
        
        uint64_t sid = std::strtoull(surfaceId, nullptr, 0);
        OHNativeWindow *window = nullptr;
        int ret = OH_NativeWindow_CreateNativeWindowFromSurfaceId(sid, &window);
        if(ret == 0 && window)
        {
            native->removeSurface(window);    
        }
        
        delete [] surfaceId;
    }
    return nullptr; // return void
}
static napi_value NAPI_Global_nativePrepare(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status == napi_ok) {

        int64_t ptr;
        napi_get_value_int64(env, args[0], &ptr);

        HelloPlayer *native = reinterpret_cast<HelloPlayer *>(ptr);

        int64_t ptsUs;
        napi_get_value_int64(env, args[1], &ptsUs);
        
        native->prepare(ptsUs);
    }
    return nullptr; // return void
}
static napi_value NAPI_Global_nativeStart(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status == napi_ok) {

        int64_t ptr;
        napi_get_value_int64(env, args[0], &ptr);

        HelloPlayer *native = reinterpret_cast<HelloPlayer *>(ptr);
        native->start();
    }
    return nullptr; // return void
}
static napi_value NAPI_Global_nativePause(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status == napi_ok) {

        int64_t ptr;
        napi_get_value_int64(env, args[0], &ptr);

        HelloPlayer *native = reinterpret_cast<HelloPlayer *>(ptr);
        native->pause();
    }
    return nullptr; // return void
}
static napi_value NAPI_Global_nativeSeekTo(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status == napi_ok) {

        int64_t ptr;
        napi_get_value_int64(env, args[0], &ptr);

        HelloPlayer *native = reinterpret_cast<HelloPlayer *>(ptr);

        int64_t ptsUs;
        napi_get_value_int64(env, args[1], &ptsUs);
        
        bool autoPlay = false;
        napi_get_value_bool(env, args[2], &autoPlay);

        native->seekTo(ptsUs, autoPlay);
    }
    return nullptr; // return void
}
static napi_value NAPI_Global_nativeSetVolume(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status == napi_ok) {

        int64_t ptr;
        napi_get_value_int64(env, args[0], &ptr);

        HelloPlayer *native = reinterpret_cast<HelloPlayer *>(ptr);

        double volume;
        napi_get_value_double(env, args[1], &volume);
        
        native->setVolume(volume);
    }
    return nullptr; // return void
}
static napi_value NAPI_Global_nativeSetSpeed(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status == napi_ok) {

        int64_t ptr;
        napi_get_value_int64(env, args[0], &ptr);

        HelloPlayer *native = reinterpret_cast<HelloPlayer *>(ptr);

        double speed;
        napi_get_value_double(env, args[1], &speed);
        
        native->setSpeed(speed);
    }
    return nullptr; // return void
}
static napi_value NAPI_Global_nativeReset(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status == napi_ok) {

        int64_t ptr;
        napi_get_value_int64(env, args[0], &ptr);

        HelloPlayer *native = reinterpret_cast<HelloPlayer *>(ptr);
        native->reset();
    }
    return nullptr; // return void
}
static napi_value NAPI_Global_nativeRelease(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status == napi_ok) {

        int64_t ptr;
        napi_get_value_int64(env, args[0], &ptr);

        HelloPlayer *native = reinterpret_cast<HelloPlayer *>(ptr);
        delete native;
    }
    return nullptr; // return void
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"nativeInit", nullptr, NAPI_Global_nativeInit, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"nativeSetConfig", nullptr, NAPI_Global_nativeSetConfig, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"nativeGetConfig", nullptr, NAPI_Global_nativeGetConfig, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"nativeSetDataSource", nullptr, NAPI_Global_nativeSetDataSource, nullptr, nullptr, nullptr, napi_default,
         nullptr},
        {"nativeAddSurface", nullptr, NAPI_Global_nativeAddSurface, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"nativeRemoveSurface", nullptr, NAPI_Global_nativeRemoveSurface, nullptr, nullptr, nullptr, napi_default,
         nullptr},
        {"nativePrepare", nullptr, NAPI_Global_nativePrepare, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"nativeStart", nullptr, NAPI_Global_nativeStart, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"nativePause", nullptr, NAPI_Global_nativePause, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"nativeSeekTo", nullptr, NAPI_Global_nativeSeekTo, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"nativeSetVolume", nullptr, NAPI_Global_nativeSetVolume, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"nativeSetSpeed", nullptr, NAPI_Global_nativeSetSpeed, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"nativeReset", nullptr, NAPI_Global_nativeReset, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"nativeRelease", nullptr, NAPI_Global_nativeRelease, nullptr, nullptr, nullptr, napi_default, nullptr},

    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "helloplayer",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterHelloplayerModule(void) {
    // 注册log日志能力
    // NativeHandler 配置log打印
    NHLog::instance()->logPrinter = onLogPrint;
    Logger::logPrinter  =  onLogPrint2;
    Logger::enable = true;
    
    
    napi_module_register(&demoModule);
}
