//
// Created on 2025/3/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OHOS_NAPICALLBACKDATA_H
#define OHOS_NAPICALLBACKDATA_H

#include <cstdint>

typedef struct {
    int64_t long0;
    int64_t long1;
    int64_t long2;
    int64_t long3;
    int64_t long4;
    int64_t long5;
    int64_t long6;
    int64_t long7;
    int64_t long8;
    int64_t long9;

    const char * str0;
    
    bool bool0;
} NapiCallbackData;

#endif //OHOS_NAPICALLBACKDATA_H
