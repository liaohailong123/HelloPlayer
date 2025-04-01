//
// Created by liaohailong on 2024/10/12.
//
#define LOG_TAG "Victor"

#include "JniInit.hpp"
#include <string>

#include "framework/log/Logger.hpp"
#include "framework/util/FFUtil.hpp"
#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h>

typedef struct {
    SpeexEchoState *st;  // 声明Speex回声抑制状态
    SpeexPreprocessState *den;  // 声明Speex音频预处理（降噪等）状态
} AudioState;


jlong nativeInitSpeexDSP(JNIEnv *env, jobject thiz, jint frame_size,
                         jint sampling_rate, jint filter_length) {
    AudioState *state = new AudioState();

    // 初始化回声抑制状态和音频预处理状态
    state->st = speex_echo_state_init(frame_size, filter_length);  // 初始化回声抑制状态，传入每帧大小和缓冲区大小
    state->den = speex_preprocess_state_init(frame_size, sampling_rate);  // 初始化音频预处理状态，传入每帧大小和采样率

    // 设置回声抑制的采样率
    speex_echo_ctl(state->st, SPEEX_ECHO_SET_SAMPLING_RATE, &sampling_rate);
    // 将回声抑制状态传递给音频预处理器，用于去回声
    speex_preprocess_ctl(state->den, SPEEX_PREPROCESS_SET_ECHO_STATE, state->st);

    // 设置降噪和回声消除的参数
    int i = 1;  // 启用降噪
    speex_preprocess_ctl(state->den, SPEEX_PREPROCESS_SET_DENOISE, &i);
    i=0;
    speex_preprocess_ctl(state->den, SPEEX_PREPROCESS_SET_AGC, &i);
    i=8000;
    speex_preprocess_ctl(state->den, SPEEX_PREPROCESS_SET_AGC_LEVEL, &i);
    i=0;
    speex_preprocess_ctl(state->den, SPEEX_PREPROCESS_SET_DEREVERB, &i);
    float f=.0;
    speex_preprocess_ctl(state->den, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
    f=.0;
    speex_preprocess_ctl(state->den, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);

    return reinterpret_cast<jlong>(state);
}

jshortArray nativePreprocess(JNIEnv *env, jobject thiz, jlong ptr,
                          jshortArray playback, jshortArray capture, jint frame_size) {
    auto state = reinterpret_cast<AudioState *>(ptr);
    jshortArray output = env->NewShortArray(frame_size);

    jshort *_output = env->GetShortArrayElements(output, nullptr);
    jshort *_playback = env->GetShortArrayElements(playback, nullptr);
    jshort *_capture = env->GetShortArrayElements(capture, nullptr);

    // 进行回声抑制，传入参考信号、带回声的信号，输出去回声后的信号
    speex_echo_cancellation(state->st, _capture, _playback, _output);

    // 进行音频预处理（如降噪），处理去回声后的信号
    speex_preprocess_run(state->den, _output);

    // 释放 JNI 数组元素指针，保证内存管理正确
    env->ReleaseShortArrayElements(output, _output, 0);
    env->ReleaseShortArrayElements(playback, _playback, 0);
    env->ReleaseShortArrayElements(capture, _capture, 0);

    return output;
}

void nativeRelease(JNIEnv *env, jobject thiz, jlong ptr) {
    auto state = reinterpret_cast<AudioState *>(ptr);
    speex_echo_state_destroy(state->st);
    speex_preprocess_state_destroy(state->den);
    delete state;
}


static JNINativeMethod jniMethods[] = {
        {"nativeInit",       "(III)J",    (void *) nativeInitSpeexDSP},
        {"nativePreprocess", "(J[S[SI)[S", (void *) nativePreprocess},
        {"nativeRelease",    "(J)V",      (void *) nativeRelease},
};


int register_rtsp_speex_dsp(JNIEnv *env) {
    jclass clz = env->FindClass("com/example/module_rtsp/util/SpeexDSP");
    if (clz == nullptr) {
        return false;
    }

    int num = ((int) sizeof(jniMethods) / sizeof(jniMethods[0]));
    jint status = env->RegisterNatives(clz, jniMethods, num);
    if (status < JNI_OK) {
        Logger(LOG_TAG).i("RegisterNatives failure");
    }

    env->DeleteLocalRef(clz);
    return status == JNI_OK;
}