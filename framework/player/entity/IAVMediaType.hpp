//
// Created by liaohailong on 2024/12/19.
//

#ifndef HELLOPLAYER_AVPACKETTYPE_HPP
#define HELLOPLAYER_AVPACKETTYPE_HPP

/**
 * 多媒体数据类型
 */
typedef enum
{
    /**
     * 未知数据
     */
    UNKNOWN = -1,
    /**
     * 音频数据
     */
    AUDIO,
    /**
     * 视频数据
     */
    VIDEO
} IAVMediaType;


#endif //HELLOPLAYER_AVPACKETTYPE_HPP
