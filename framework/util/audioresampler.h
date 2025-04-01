#ifndef AUDIORESAMPLER_H
#define AUDIORESAMPLER_H

#include <libavutil/audio_fifo.h>
#include <libavutil/opt.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
#include <libavutil/error.h>
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>

// 重采样的参数
typedef struct audio_resampler_params {
    // 输入参数
    enum AVSampleFormat src_sample_fmt;
    int src_sample_rate;
    AVChannelLayout src_channel_layout;
    // 输出参数
    enum AVSampleFormat dst_sample_fmt;
    int dst_sample_rate;
    AVChannelLayout dst_channel_layout;
}audio_resampler_params_t;

// 封装的重采样器
typedef struct audio_resampler {
    struct SwrContext *swr_ctx;     // 重采样的核心
    audio_resampler_params_t resampler_params;  // 重采样的设置参数
    int is_fifo_only;       // 不需要进行重采样，只需要缓存到 audio_fifo
    int is_flushed;         // flush的时候使用
    AVAudioFifo *audio_fifo;    // 采样点的缓存
    int64_t in_start_pts;          // 进入的起始pts
    int64_t in_last_pts;           // 保存上一次的pts时间戳
    int64_t out_start_pts;          // 输出的pts
    int64_t out_cur_pts;            // 当前pts


    uint8_t **resampled_data;   // 用来缓存重采样后的数据
    int resampled_data_size;    // 重采样后的采样数
    int src_channels;           // 输入的通道数
    int dst_channels;           // 输出通道数
    int64_t total_resampled_num;    // 统计总共的重采样点数，目前只是统计
}audio_resampler_t;

/**
 * @brief 分配重采样器
 * @param resampler_params 重采样的设置参数
 * @return 成功返回重采样器；失败返回NULL
 */
audio_resampler_t *audio_resampler_alloc(const audio_resampler_params_t resampler_params);

/**
 * @brief 释放重采样器
 * @param resampler
 */
void audio_resampler_free(audio_resampler_t *resampler);

/**
 * @brief 发送要进行重采样的帧
 * @param resampler
 * @param frame
 * @return 这次重采样后得到的采样点数
 */
int audio_resampler_send_frame(audio_resampler_t *resampler, AVFrame *frame);

/**
 * @brief 发送要进行重采样的帧
 * @param resampler
 * @param in_data 二级指针
 * @param in_nb_samples 输入的采样点数量(单个通道)
 * @param pts       pts
 * @return
 */
int audio_resampler_send_frame2(audio_resampler_t *resampler, uint8_t **in_data,int in_nb_samples, int64_t pts);

/**
 * @brief 发送要进行重采样的帧
 * @param resampler
 * @param in_data 一级指针
 * @param in_bytes 传入数据的字节大小
 * @param pts
 * @return
 */
int audio_resampler_send_frame3(audio_resampler_t *resampler, uint8_t *in_data,int in_bytes, int64_t pts);

/**
 * @brief 获取重采样后的数据
 * @param resampler
 * @param nb_samples    我们需要读取多少采样数量: 如果nb_samples>0，只有audio fifo>=nb_samples才能获取到数据;nb_samples=0,有多少就给多少
 *
 * @return 如果获取到采样点数则非NULL
 */
AVFrame *audio_resampler_receive_frame(audio_resampler_t *resampler, int nb_samples);


/**
 * @brief 获取重采样后的数据
 * @param resampler
 * @param out_data 缓存获取到的重采样数据，buf一定要充足
 * @param nb_samples 我们需要读取多少采样数量: 如果nb_samples>0，只有audio fifo>=nb_samples才能获取到数据;nb_samples=0,有多少就给多少
 * @param pts  获取帧的pts值
 * @return 获取到的采样点数量
 */
int  audio_resampler_receive_frame2(audio_resampler_t *resampler, uint8_t **out_data, int nb_samples, int64_t *pts);

/**
 * @brief audio_resampler_get_fifo_size
 * @param resampler
 * @return audio fifo的缓存的采样点数量
 */
int audio_resampler_get_fifo_size(audio_resampler_t *resampler);
/**
 * @brief audio_resampler_get_start_pts
 * @param resampler
 * @return 起始的pts
 */
int64_t audio_resampler_get_start_pts(audio_resampler_t *resampler);
/**
 * @brief audio_resampler_get_cur_pts
 * @param resampler
 * @return 当前的pts
 */
int64_t audio_resampler_get_cur_pts(audio_resampler_t *resampler);


#endif // AUDIORESAMPLER_H
