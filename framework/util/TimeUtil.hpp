//
// Created by 廖海龙 on 2024/11/18.
//

#ifndef ANDROID_TIMEUTIL_HPP
#define ANDROID_TIMEUTIL_HPP

#include <stdlib.h>
#include <chrono>


/**
 * Author: liaohailong
 * Date: 2024/11/18
 * Time: 20:16
 * Description:
 **/
namespace TimeUtil {

    /**
     * @return 获取当前时间戳，单位：ms
     */
    int64_t getCurrentTimeMs();
    
    /**
     * @return 获取当前时间戳，单位：us
     */
    int64_t getCurrentTimeUs();

}


#endif //ANDROID_TIMEUTIL_HPP
