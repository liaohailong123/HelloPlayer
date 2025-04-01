//
// Created by liaohailong on 2024/10/12.
//
#define LOG_TAG "Victor"

#include "JniInit.hpp"
#include <string>

#include "framework/log/Logger.hpp"
#include "framework/util/FFUtil.hpp"
#include "framework/util/h26x_sps_parse/h26x_sps_parse.h"


jobject nativeParseSPS(JNIEnv *env, jclass thiz, jbyteArray sps) {

    jclass clazz = env->FindClass("com/example/module_rtsp/util/SPSInfo");

    if (clazz == nullptr) {
        return nullptr;
    }
    jmethodID constructor = env->GetMethodID(clazz, "<init>",
                                             "(II)V");
    jobject obj = nullptr;

    jbyte *buffer = env->GetByteArrayElements(sps, nullptr);
    int len = env->GetArrayLength(sps);

    const uint8_t *data = reinterpret_cast<const uint8_t *>(buffer);
    monk::H26xSPSInfo info{};
    int ret = monk::H26xParseSPS::h264Parse(data, len, info);

    if (ret == 0) {
        obj = env->NewObject(clazz, constructor, (jint) info.width, (jint) info.height);
    } else {
        obj = env->NewObject(clazz, constructor, 0, 0);
    }

    env->DeleteLocalRef(clazz);
    env->ReleaseByteArrayElements(sps, buffer, 0);
    return obj;
}


static JNINativeMethod jniMethods[] = {
        {"nativeParseSPS", "([B)Lcom/example/module_rtsp/util/SPSInfo;", (void *) nativeParseSPS},
};


int register_rtsp_h264_parser(JNIEnv *env) {

    jclass clz = env->FindClass("com/example/module_rtsp/util/H264SPSParser");
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