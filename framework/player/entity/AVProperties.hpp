//
// Created by 廖海龙 on 2024/11/7.
//

#ifndef HLSDEMO_AVPROPERTIES_HPP
#define HLSDEMO_AVPROPERTIES_HPP


#include "../../log/Logger.hpp"
#include <string>

#define AUDIO_ENCODE_AAC "aac"
#define VIDEO_ENCODE_AV1 "av1"
#define VIDEO_ENCODE_H264 "h264"
#define VIDEO_ENCODE_H265 "h265"

// I帧标识
#define BUFFER_FLAG_KEY_FRAME 1

typedef enum AUDIO_SAMPLE_FMT {
// 音频样本位深类型
    ENCODING_INVALID = 0,
/** Default audio data format */
    ENCODING_DEFAULT = 1,

// These values must be kept in sync with core/jni/android_media_AudioFormat.h
// Also sync av/services/audiopolicy/managerdefault/ConfigParsingUtils.h
/** Audio data format: PCM 16 bit per sample. Guaranteed to be supported by devices. */
    ENCODING_PCM_16BIT = 2,
/** Audio data format: PCM 8 bit per sample. Not guaranteed to be supported by devices. */
    ENCODING_PCM_8BIT = 3,
/** Audio data format: single-precision floating-point per sample */
    // packet 32 float L R L R L R L R L R L R L R L R ...
    ENCODING_PCM_FLOAT = 4,
    // planner 32 float  L L L L .... / R R R R ....
    ENCODING_PCM_FLOAT_P = 5,

} AUDIO_SAMPLE_FMT;


// 音频声道数 MediaFormat.java
#define CHANNEL_IN_LEFT 0x4
#define CHANNEL_IN_RIGHT 0x8
#define CHANNEL_IN_FRONT 0x10
#define CHANNEL_IN_BACK 0x20
#define CHANNEL_IN_MONO CHANNEL_IN_FRONT
#define CHANNEL_IN_STEREO (CHANNEL_IN_LEFT | CHANNEL_IN_RIGHT)

typedef struct {
    /**
     * 采样率
     */
    int sampleRate;
    /**
     * 声道数
     */
    int channelCount;
    /**
     * 位深
     */
    int sampleFmt;

    /**
     * 比特率，直接影响音频编码数据大小
     */
    int bitrate;

    /**
     * 音频编码格式
     */
    std::string mimeType;

    /**
     * 一帧音频数据包含多少个样本
     */
    int frame_size;
} AudioProperties;

typedef struct {
    /**
     * 视频编码格式
     */
    std::string mimeType;
    /**
     * 视频宽
     */
    int width;
    /**
     * 视频高
     */
    int height;
    /**
     * 帧率
     */
    int fps;
    /**
     * 比特率
     */
    int bitrate;
    /**
     * 几秒钟一个I帧
     */
    int iFrameInternal;
    /**
     * 编码源数据类型
     * ffmpeg： AV_PIX_FMT_YUV420P
     */
    int colorFormat;
} VideoProperties;


#endif //HLSDEMO_AVPROPERTIES_HPP
