//
// Created by liaohailong on 2024/10/12.
//
#define LOG_TAG "Victor"

#include "JniInit.hpp"
#include <string>
#include <mutex>

#include <android/native_window.h> // Android上层android.view.Surface
#include <android/native_window_jni.h> // Surface 与 ANativeWindow 相互转换

#include "framework/log/Logger.hpp"
#include "framework/util/FFUtil.hpp"
#include "framework/player/HelloPlayer.hpp"


jlong nativeInitHelloPlayer(JNIEnv *env, jobject thiz)
{
    jobject globalObj = env->NewGlobalRef(thiz);

    auto native = new HelloPlayer(globalObj);
    return reinterpret_cast<jlong>(native);
}

void nativeSetConfig(JNIEnv *env, jobject thiz, jlong ptr, jobject config)
{
    if (config == nullptr)
    {
        // 如果 config 为 null，直接返回
        return;
    }

    jclass helloPlayerConfigClass = env->GetObjectClass(config);
    if (helloPlayerConfigClass == nullptr)
    {
        // 如果获取类失败，直接返回
        return;
    }

    jfieldID bufferDurationUsField = env->GetFieldID(helloPlayerConfigClass, "bufferDurationUs",
                                                     "J");
    jfieldID bufferLoadMoreFactorField = env->GetFieldID(helloPlayerConfigClass,
                                                         "bufferLoadMoreFactor",
                                                         "D");
    jfieldID useHardwareField = env->GetFieldID(helloPlayerConfigClass, "useHardware", "Z");
    jfieldID autoPlayField = env->GetFieldID(helloPlayerConfigClass, "autoPlay", "Z");
    jfieldID loopPlayField = env->GetFieldID(helloPlayerConfigClass, "loopPlay", "Z");
    jfieldID volumeField = env->GetFieldID(helloPlayerConfigClass, "volume", "D");
    jfieldID speedField = env->GetFieldID(helloPlayerConfigClass, "speed", "D");
    jfieldID ioTimeoutUsField = env->GetFieldID(helloPlayerConfigClass, "ioTimeoutUs", "J");

    // 检查是否成功获取字段 ID
    if (bufferDurationUsField == nullptr || bufferLoadMoreFactorField == nullptr ||
        autoPlayField == nullptr || loopPlayField == nullptr)
    {
        return;  // 如果获取字段 ID 失败，直接返回
    }


    // 获取字段值
    jlong bufferDurationUs = env->GetLongField(config, bufferDurationUsField);
    jdouble bufferLoadMoreFactor = env->GetDoubleField(config, bufferLoadMoreFactorField);
    jboolean useHardware = env->GetBooleanField(config, useHardwareField);
    jboolean autoPlay = env->GetBooleanField(config, autoPlayField);
    jboolean loopPlay = env->GetBooleanField(config, loopPlayField);
    jdouble volume = env->GetDoubleField(config, volumeField);
    jdouble speed = env->GetDoubleField(config, speedField);
    jlong ioTimeoutUs = env->GetLongField(config, ioTimeoutUsField);

    // 检查 ptr 是否有效
    if (ptr == 0)
    {
        return;  // 如果 ptr 为 null，则返回
    }

    auto native = reinterpret_cast<HelloPlayer *>(ptr);

    PlayConfig playConfig;
    playConfig.bufferDurationUs = bufferDurationUs;
    playConfig.bufferLoadMoreFactor = bufferLoadMoreFactor;
    playConfig.useHardware = useHardware;
    playConfig.autoPlay = autoPlay;
    playConfig.loopPlay = loopPlay;
    playConfig.volume = volume;
    playConfig.speed = speed;
    playConfig.ioTimeoutUs = ioTimeoutUs;

    native->setConfig(playConfig);
}

jobject nativeGetConfig(JNIEnv *env, jobject thiz, jlong ptr)
{
    // 检查 ptr 是否有效
    if (ptr == 0)
    {
        return nullptr;  // 如果 ptr 为 null，返回 null
    }

    // 获取 native 对象
    auto native = reinterpret_cast<HelloPlayer *>(ptr);

    // 获取 native 配置信息（假设 PlayConfig 是包含配置的结构体）
    PlayConfig playConfig = native->getConfig();  // 获取配置

    // 获取 HelloPlayerConfig 类引用
    jclass helloPlayerConfigClass = env->FindClass("com/example/module_rtsp/HelloPlayerConfig");
    if (helloPlayerConfigClass == nullptr)
    {
        return nullptr;  // 找不到类，返回 null
    }

    // 获取构造函数 ID
    jmethodID constructor = env->GetMethodID(helloPlayerConfigClass, "<init>", "()V");
    if (constructor == nullptr)
    {
        return nullptr;  // 找不到构造函数，返回 null
    }

    // 创建 HelloPlayerConfig Java 对象
    jobject configObject = env->NewObject(helloPlayerConfigClass, constructor);
    if (configObject == nullptr)
    {
        return nullptr;  // 创建对象失败，返回 null
    }

    // 获取字段 ID
    jfieldID bufferDurationUsField = env->GetFieldID(helloPlayerConfigClass, "bufferDurationUs",
                                                     "J");
    jfieldID bufferLoadMoreFactorField = env->GetFieldID(helloPlayerConfigClass,
                                                         "bufferLoadMoreFactor", "D");
    jfieldID autoPlayField = env->GetFieldID(helloPlayerConfigClass, "autoPlay", "Z");
    jfieldID loopPlayField = env->GetFieldID(helloPlayerConfigClass, "loopPlay", "Z");
    jfieldID volumeField = env->GetFieldID(helloPlayerConfigClass, "volume", "D");
    jfieldID speedField = env->GetFieldID(helloPlayerConfigClass, "speed", "D");

    if (bufferDurationUsField == nullptr || bufferLoadMoreFactorField == nullptr ||
        autoPlayField == nullptr || loopPlayField == nullptr)
    {
        return nullptr;  // 如果字段 ID 获取失败，返回 null
    }

    // 设置字段值
    env->SetLongField(configObject, bufferDurationUsField, playConfig.bufferDurationUs);
    env->SetDoubleField(configObject, bufferLoadMoreFactorField, playConfig.bufferLoadMoreFactor);
    env->SetBooleanField(configObject, autoPlayField, playConfig.autoPlay);
    env->SetBooleanField(configObject, loopPlayField, playConfig.loopPlay);
    env->SetDoubleField(configObject,volumeField,playConfig.volume);
    env->SetDoubleField(configObject,speedField,playConfig.speed);

    // 返回 configObject（HelloPlayerConfig Java 对象）
    return configObject;
}

void nativeSetDataSource(JNIEnv *env, jobject thiz, jlong ptr, jstring url)
{
    auto native = reinterpret_cast<HelloPlayer *>(ptr);
    const char *c_url = env->GetStringUTFChars(url, nullptr);
    native->setDataSource(c_url);
    env->ReleaseStringUTFChars(url, c_url);
}

void nativeAddSurface(JNIEnv *env, jobject thiz, jlong ptr, jobject surface)
{
    auto native = reinterpret_cast<HelloPlayer *>(ptr);
    native->addSurface(ANativeWindow_fromSurface(env, surface));
}

void nativeRemoveSurface(JNIEnv *env, jobject thiz, jlong ptr, jobject surface)
{
    auto native = reinterpret_cast<HelloPlayer *>(ptr);
    native->removeSurface(ANativeWindow_fromSurface(env, surface));
}


void nativePrepare(JNIEnv *env, jobject thiz, jlong ptr, jlong ptsUs)
{
    auto native = reinterpret_cast<HelloPlayer *>(ptr);
    native->prepare(ptsUs);
}

void nativeStart(JNIEnv *env, jobject thiz, jlong ptr)
{
    auto native = reinterpret_cast<HelloPlayer *>(ptr);
    native->start();
}

void nativePause(JNIEnv *env, jobject thiz, jlong ptr)
{
    auto native = reinterpret_cast<HelloPlayer *>(ptr);
    native->pause();
}

void nativeSeekTo(JNIEnv *env, jobject thiz, jlong ptr, jlong ptsUs, jboolean autoPlay)
{
    auto native = reinterpret_cast<HelloPlayer *>(ptr);
    native->seekTo(ptsUs, autoPlay);
}

void nativeSetVolume(JNIEnv *env, jobject thiz, jlong ptr, jdouble volume)
{
    auto native = reinterpret_cast<HelloPlayer *>(ptr);
    native->setVolume(volume);
}

void nativeSetSpeed(JNIEnv *env, jobject thiz, jlong ptr, jdouble speed)
{
    auto native = reinterpret_cast<HelloPlayer *>(ptr);
    native->setSpeed(speed);
}

void nativeReset(JNIEnv *env, jobject thiz, jlong ptr)
{
    auto native = reinterpret_cast<HelloPlayer *>(ptr);
    native->reset();
}


void nativeReleaseHelloPlayer(JNIEnv *env, jobject thiz, jlong ptr)
{
    auto native = reinterpret_cast<HelloPlayer *>(ptr);
    delete native;
}


static JNINativeMethod jniMethods[] = {
        {"nativeInit",          "()J",                        (void *) nativeInitHelloPlayer},

        {"nativeSetConfig",     "(JLcom/example/module_rtsp/HelloPlayerConfig;)V",
                                                              (void *) nativeSetConfig},
        {"nativeGetConfig",     "(J)Lcom/example/module_rtsp/HelloPlayerConfig;",
                                                              (void *) nativeGetConfig},
        {"nativeSetDataSource", "(JLjava/lang/String;)V",     (void *) nativeSetDataSource},
        {"nativeAddSurface",    "(JLandroid/view/Surface;)V", (void *) nativeAddSurface},
        {"nativeRemoveSurface", "(JLandroid/view/Surface;)V", (void *) nativeRemoveSurface},
        {"nativePrepare",       "(JJ)V",                      (void *) nativePrepare},
        {"nativeStart",         "(J)V",                       (void *) nativeStart},
        {"nativePause",         "(J)V",                       (void *) nativePause},
        {"nativeSeekTo",        "(JJZ)V",                     (void *) nativeSeekTo},
        {"nativeSetVolume",     "(JD)V",                      (void *) nativeSetVolume},
        {"nativeSetSpeed",      "(JD)V",                      (void *) nativeSetSpeed},
        {"nativeReset",         "(J)V",                       (void *) nativeReset},



        {"nativeRelease",       "(J)V",                       (void *) nativeReleaseHelloPlayer},
};


int register_hello_player(JNIEnv *env)
{

    jclass clz = env->FindClass("com/example/module_rtsp/HelloPlayer");
    if (clz == nullptr)
    {
        return false;
    }

    int num = ((int) sizeof(jniMethods) / sizeof(jniMethods[0]));
    jint status = env->RegisterNatives(clz, jniMethods, num);
    if (status < JNI_OK)
    {
        Logger(LOG_TAG).i("RegisterNatives failure");
    }

    env->DeleteLocalRef(clz);

    return status == JNI_OK;
}