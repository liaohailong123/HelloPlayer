//
// Created by 廖海龙 on 2025/2/28.
//

#include "HelloAndroidCallback.hpp"

HelloAndroidCallback::HelloAndroidCallback(void* instance) : HelloPlayerCallback(instance)
{
    globalObj = static_cast<jobject>(instance);
    JNIEnv *env = Jni::getJNIEnv();
    if (env)
    {
        // 从java调用线程中拿到class，因为c++层创建的子线程中，java classloader 可能加载不到类
        jclass mediaInfoClz = env->FindClass("com/example/module_rtsp/HelloPlayerMediaInfo");
        mediaInfoClass = static_cast<jclass>(env->NewGlobalRef(mediaInfoClz));
        jclass clz = env->GetObjectClass(globalObj);
        globalObjClz = (jclass)env->NewGlobalRef(clz);
    }

    logger.i("HelloAndroidCallback::HelloAndroidCallback(%p)", this);
}

HelloAndroidCallback::~HelloAndroidCallback()
{
    logger.i("HelloAndroidCallback::~HelloAndroidCallback(%p)", this);
}

void HelloAndroidCallback::onPrepared(const Hello::MediaInfo &info)
{
    if (globalObj == nullptr) return;

    JNIEnv *env = Jni::getJNIEnv();
    if (env == nullptr) return;

    if (globalObjClz == nullptr)
    {
        logger.i("globalObjClz not found");
        return;
    }

    jmethodID onPreparedMethodId = env->GetMethodID(globalObjClz, "onPrepared",
                                                    "(Lcom/example/module_rtsp/HelloPlayerMediaInfo;)V");
    if (onPreparedMethodId == nullptr)
    {
        logger.i("onPreparedMethodId not found");
        return;
    }

    if (mediaInfoClass == nullptr)
    {
        logger.i("HelloPlayerMediaInfo class not found");
        return;
    }

    // 获取构造函数
    jmethodID constructor = env->GetMethodID(mediaInfoClass, "<init>",
                                             "(Ljava/lang/String;JJJIIIIIIIZ)V");
    if (constructor == nullptr)
    {
        logger.i("Constructor not found");
        return;
    }

    // 准备构造函数所需字段
    const char *c_url = info.url.c_str();
    jstring url = env->NewStringUTF(c_url);
    int frameRateNum = info.frameRate.num;

    jobject obj = env->NewObject(mediaInfoClass, constructor,
                                 url,
                                 info.startOffsetUs,
                                 info.audioDurationUs,
                                 info.videoDurationUs,
                                 info.sampleRate, info.channelCount, info.sampleFmt,
                                 info.width, info.height, frameRateNum, info.masterClockType,
                                 info.isLiveStreaming
    );

    // 删除局部引用
    env->DeleteLocalRef(url);


    // 调用 onPrepared 方法
    env->CallVoidMethod(globalObj, onPreparedMethodId, obj);

    // 检查异常
    if (env->ExceptionCheck())
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}

/**
 * 播放状态改变 HelloPlayer.cpp::PlayState
 * @param from 从这个状态
 * @param to 变到这个状态
 */
void HelloAndroidCallback::onPlayStateChanged(int from, int to)
{
    if (globalObj == nullptr) return;

    JNIEnv *env = Jni::getJNIEnv();
    if (env == nullptr) return;

    if (globalObjClz == nullptr)
    {
        logger.i("globalObjClz not found");
        return;
    }

    jmethodID methodId = env->GetMethodID(globalObjClz, "onPlayStateChanged",
                                          "(II)V");
    if (methodId == nullptr)
    {
        logger.i("onPlayStateChanged method id not found");
        return;
    }

    // 调用方法
    env->CallVoidMethod(globalObj, methodId, from, to);

    // 检查异常
    if (env->ExceptionCheck())
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}

/**
 * 错误信息回调
 * @param error 错误码 对应 HelloPlayer::ErrorCode
 * @param msg 错误信息
 */
void HelloAndroidCallback::onErrorInfo(int error, const char* msg)
{
    if (globalObj == nullptr) return;

    JNIEnv *env = Jni::getJNIEnv();
    if (env == nullptr) return;

    if (globalObjClz == nullptr)
    {
        logger.i("globalObjClz not found");
        return;
    }

    jmethodID methodId = env->GetMethodID(globalObjClz, "onErrorInfo",
                                          "(ILjava/lang/String;)V");
    if (methodId == nullptr)
    {
        logger.i("onErrorInfo method id not found");
        return;
    }


    jstring message = env->NewStringUTF(msg);

    // 调用方法
    env->CallVoidMethod(globalObj, methodId, error, message);

    // 删除局部引用
    env->DeleteLocalRef(message);

    // 检查异常
    if (env->ExceptionCheck())
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}


/**
 * @param ptsUs 回调当前播放时间，单位：us
 */
void HelloAndroidCallback::onCurrentPosition(int64_t ptsUs, int64_t durationUs)
{
    if (globalObj == nullptr) return;

    JNIEnv *env = Jni::getJNIEnv();
    if (env == nullptr) return;

    if (globalObjClz == nullptr)
    {
        logger.i("globalObjClz not found");
        return;
    }

    jmethodID methodId = env->GetMethodID(globalObjClz, "onCurrentPosition",
                                          "(JJ)V");
    if (methodId == nullptr)
    {
        logger.i("onCurrentPosition method id not found");
        return;
    }

    // 调用方法
    env->CallVoidMethod(globalObj, methodId, ptsUs, durationUs);

    // 检查异常
    if (env->ExceptionCheck())
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }

}

/**
 * 回调当前缓冲区范围
 * @param startUs 缓冲区起始时间，单位：us
 * @param endUs 缓冲区结束时间，单位：us
 */
void HelloAndroidCallback::onBufferPosition(int64_t startUs, int64_t endUs, int64_t durationUs)
{

    if (globalObj == nullptr) return;

    JNIEnv *env = Jni::getJNIEnv();
    if (env == nullptr) return;

    if (globalObjClz == nullptr)
    {
        logger.i("globalObjClz not found");
        return;
    }

    jmethodID methodId = env->GetMethodID(globalObjClz, "onBufferPosition",
                                          "(JJJ)V");
    if (methodId == nullptr)
    {
        logger.i("onBufferPosition method id not found");
        return;
    }

    // 调用方法
    env->CallVoidMethod(globalObj, methodId, startUs, endUs, durationUs);

    // 检查异常
    if (env->ExceptionCheck())
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}