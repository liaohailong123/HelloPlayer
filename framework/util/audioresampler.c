#include "audioresampler.h"

static AVFrame *audio_resampler_alloc_out_frame(const int nb_samples, const audio_resampler_params_t *resampler_params)
{
    int ret;
    AVFrame * frame = av_frame_alloc();
    if(!frame)
    {
        return NULL;
    }
    frame->nb_samples = nb_samples;
    frame->ch_layout = resampler_params->dst_channel_layout;
    frame->format = resampler_params->dst_sample_fmt;
    frame->sample_rate = resampler_params->dst_sample_rate;
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0)
    {
//        printf("cannot allocate audio data buffer\n");
        return NULL;
    }
    return frame;
}

static AVFrame *get_one_frame(audio_resampler_t *resampler, const int nb_samples)
{
    AVFrame * frame = audio_resampler_alloc_out_frame(nb_samples, &resampler->resampler_params);
    if (frame)
    {
        av_audio_fifo_read(resampler->audio_fifo, (void **)frame->data, nb_samples);
        frame->pts = resampler->out_cur_pts;
        resampler->out_cur_pts += nb_samples;
        resampler->total_resampled_num += nb_samples;
    }
    return frame;
}

static int audio_resampler_init_data(audio_resampler_t *resampler)
{
    if (resampler->resampled_data)
        av_freep(&resampler->resampled_data[0]);
    av_freep(&resampler->resampled_data);       // av_freep即使传入NULL也是安全的
    int linesize = 0;
    int ret=  av_samples_alloc_array_and_samples(&resampler->resampled_data, &linesize, resampler->dst_channels,
                                                 resampler->resampled_data_size, resampler->resampler_params.dst_sample_fmt, 0);
    if (ret < 0)
    {
//        printf("fail accocate audio resampled data buffer\n");
    }

    return ret;
}
// 重置时间戳
static void audio_resampler_reset_pts(audio_resampler_t *resampler, int64_t pts)
{
//    printf("audio_resampler_reset_pts in_start_pts:%ld\n", pts);
    resampler->in_start_pts = pts;
    resampler->in_last_pts = pts;
    float factor = 1.0 *resampler->resampler_params.dst_sample_rate / resampler->resampler_params.src_sample_rate;
    int64_t tmp_out_start_pts = (int64_t)(resampler->in_start_pts * factor) ;
    resampler->out_start_pts = tmp_out_start_pts;
    resampler->out_cur_pts = resampler->out_start_pts;
//    printf("audio_resampler_reset_pts out_cur_pts:%ld\n", resampler->out_cur_pts);

    int fifo_size = av_audio_fifo_size(resampler->audio_fifo);
    av_audio_fifo_drain(resampler->audio_fifo, fifo_size);
}
audio_resampler_t *audio_resampler_alloc(const audio_resampler_params_t resampler_params)
{
    int ret = 0;
    audio_resampler_t *audio_resampler = (audio_resampler_t *)av_malloc(sizeof(audio_resampler_t));
    if(!audio_resampler) {
        return NULL;
    }
    memset(audio_resampler, 0, sizeof(audio_resampler_t));
    audio_resampler->resampler_params = resampler_params;   // 设置参数
    // 设置通道数量
    audio_resampler->src_channels = resampler_params.src_channel_layout.nb_channels;
    audio_resampler->dst_channels = resampler_params.dst_channel_layout.nb_channels;
    audio_resampler->in_start_pts = AV_NOPTS_VALUE;
    audio_resampler->in_last_pts  = AV_NOPTS_VALUE;
    audio_resampler->out_start_pts = AV_NOPTS_VALUE;
    audio_resampler->out_cur_pts = AV_NOPTS_VALUE;

    // 分配audio fifo，单位其实为samples  我们在av_audio_fifo_write时候如果buffer不足能自动扩充
    audio_resampler->audio_fifo = av_audio_fifo_alloc(resampler_params.dst_sample_fmt,
                                                      audio_resampler->dst_channels, 1);
    if(!audio_resampler->audio_fifo) {
//        printf("av_audio_fifo_alloc failed\n");
        av_freep(audio_resampler);
        return NULL;
    }
    if (resampler_params.src_sample_fmt == resampler_params.dst_sample_fmt &&
            resampler_params.src_sample_rate == resampler_params.dst_sample_rate &&
            resampler_params.src_channel_layout.nb_channels == resampler_params.dst_channel_layout.nb_channels)
    {
//        printf("no resample needed, just use audio fifo\n");
        audio_resampler->is_fifo_only = 1;      // 不需要做重采样
        return audio_resampler;
    }

    // 初始化重采样
    audio_resampler->swr_ctx = swr_alloc();
    if(!audio_resampler->swr_ctx)  {
//        printf("swr_alloc failed\n");
        return NULL;
    }
    /* set options */
    av_opt_set_sample_fmt(audio_resampler->swr_ctx, "in_sample_fmt",      resampler_params.src_sample_fmt, 0);
    av_opt_set_chlayout(audio_resampler->swr_ctx,        "in_chlayout",  &resampler_params.src_channel_layout, 0);
    av_opt_set_int(audio_resampler->swr_ctx,        "in_sample_rate",     resampler_params.src_sample_rate, 0);

    av_opt_set_sample_fmt(audio_resampler->swr_ctx, "out_sample_fmt",     resampler_params.dst_sample_fmt, 0);
    av_opt_set_chlayout(audio_resampler->swr_ctx,        "out_chlayout", &resampler_params.dst_channel_layout, 0);
    av_opt_set_int(audio_resampler->swr_ctx,        "out_sample_rate",    resampler_params.dst_sample_rate, 0);

    /* initialize the resampling context */
    ret = swr_init(audio_resampler->swr_ctx);
    if (ret < 0) {
//        printf("failed to initialize the resampling context.\n");
        av_audio_fifo_free(audio_resampler->audio_fifo);
        swr_free(&audio_resampler->swr_ctx);
        av_freep(audio_resampler);
        return NULL;
    }
    audio_resampler->resampled_data_size = 8096;
    if (audio_resampler_init_data(audio_resampler) < 0) {
        swr_free(&audio_resampler->swr_ctx);
        av_audio_fifo_free(audio_resampler->audio_fifo);
        av_freep(audio_resampler);
        return NULL;
    }

//    printf("init done \n");
    return audio_resampler;
}

void audio_resampler_free(audio_resampler_t *resampler)
{
    if(!resampler) {
        return;
    }
    if (resampler->swr_ctx)
        swr_free(&resampler->swr_ctx);
    if (resampler->audio_fifo) {
        av_audio_fifo_free(resampler->audio_fifo);
    }
    if (resampler->resampled_data)
        av_freep(&resampler->resampled_data[0]);
    av_freep(&resampler->resampled_data);
    av_freep(resampler);
    resampler = NULL;
}
// 这个有支持pts的传入
int audio_resampler_send_frame(audio_resampler_t *resampler, AVFrame *frame)
{
    if(!resampler) {
        return 0;
    }
    int src_nb_samples = 0;
    uint8_t **src_data = NULL;
    if (frame)
    {
        src_nb_samples = frame->nb_samples; // 输入源采样点数量
        src_data = frame->extended_data;    // 输入源的buffer
        if (resampler->in_start_pts == AV_NOPTS_VALUE && frame->pts != AV_NOPTS_VALUE)
        {
            audio_resampler_reset_pts(resampler, frame->pts);
        }
        // 计算当前的frame pts是否有跳跃，这里不要搞那么复杂，只要超过1.5倍的差值就重置fifo和时间戳
        if((frame->pts - resampler->in_last_pts) > (int64_t)(1.5 *src_nb_samples)) {
            audio_resampler_reset_pts(resampler, frame->pts);
        }

        resampler->in_last_pts = frame->pts;
    }
    else
    {
        resampler->is_flushed = 1;
    }

    if (resampler->is_fifo_only) {
        // 如果不需要做重采样，原封不动写入fifo
        return src_data ? av_audio_fifo_write(resampler->audio_fifo, (void **)src_data, src_nb_samples) : 0;
    }
    // 计算这次做重采样能够获取到的重采样后的点数
    const int dst_nb_samples = av_rescale_rnd(swr_get_delay(resampler->swr_ctx, resampler->resampler_params.src_sample_rate) + src_nb_samples,
                                              resampler->resampler_params.dst_sample_rate ,
                                              resampler->resampler_params.src_sample_rate, AV_ROUND_UP);
    if (dst_nb_samples > resampler->resampled_data_size)
    {
        //resampled_data
        resampler->resampled_data_size = dst_nb_samples;
        if (audio_resampler_init_data(resampler) < 0) {
            return AVERROR(ENOMEM);
        }
    }
    int nb_samples = swr_convert(resampler->swr_ctx, resampler->resampled_data, dst_nb_samples,
                                 (const uint8_t **)src_data, src_nb_samples);
    // 返回实际写入的采样点数量
//    return av_audio_fifo_write(resampler->audio_fifo, (void **)resampler->resampled_data, nb_samples);
    int ret_size = av_audio_fifo_write(resampler->audio_fifo, (void **)resampler->resampled_data, nb_samples);
    if(ret_size != nb_samples) {
//        printf("Warn：av_audio_fifo_write failed, expected_write:%d, actual_write:%d\n", nb_samples, ret_size);
    }
    return ret_size;
}

AVFrame *audio_resampler_receive_frame(audio_resampler_t *resampler, int nb_samples)
{
    nb_samples = nb_samples == 0 ? av_audio_fifo_size(resampler->audio_fifo) : nb_samples;
    if (av_audio_fifo_size(resampler->audio_fifo) < nb_samples || nb_samples == 0)
        return NULL;
    // 采样点数满足条件
    return get_one_frame(resampler, nb_samples);
}

int audio_resampler_send_frame2(audio_resampler_t *resampler, uint8_t **in_data, int in_nb_samples, int64_t pts)
{
    if(!resampler) {
        return 0;
    }
    int src_nb_samples = 0;
    uint8_t **src_data = NULL;
    if (in_data)
    {
        src_nb_samples = in_nb_samples;
        src_data = in_data;
        if (resampler->in_start_pts == AV_NOPTS_VALUE && pts != AV_NOPTS_VALUE)
        {
           audio_resampler_reset_pts(resampler, pts);
        }
        if((pts -  resampler->in_last_pts) > (in_nb_samples*1.5)) {
             audio_resampler_reset_pts(resampler, pts);
        }
        resampler->in_last_pts = pts;
    }
    else
    {
        resampler->is_flushed = 1;
    }

    if (resampler->is_fifo_only) {
        return src_data ? av_audio_fifo_write(resampler->audio_fifo, (void **)src_data, src_nb_samples) : 0;
    }

    const int dst_nb_samples = av_rescale_rnd(swr_get_delay(resampler->swr_ctx, resampler->resampler_params.src_sample_rate) + src_nb_samples,
                                              resampler->resampler_params.dst_sample_rate ,
                                              resampler->resampler_params.src_sample_rate, AV_ROUND_UP);
//     printf("delay:%ld, dst_nb_samples:%d\n", swr_get_delay(resampler->swr_ctx, resampler->resampler_params.src_sample_rate), dst_nb_samples);
    if (dst_nb_samples > resampler->resampled_data_size)
    {
//        printf("%s(%d) faield,dst_nb_samples:%d, resampled_data_size:%d\n", __FUNCTION__, __LINE__, dst_nb_samples, resampler->resampled_data_size);
        //resampled_data
        resampler->resampled_data_size = dst_nb_samples;
        if (audio_resampler_init_data(resampler) < 0)
            return AVERROR(ENOMEM);
    }
    int nb_samples = swr_convert(resampler->swr_ctx, resampler->resampled_data, dst_nb_samples,
                                 (const uint8_t **)src_data, src_nb_samples);
//    printf("fifo_write:%d\n", nb_samples);
    // 返回实际写入的采样点数量
    return av_audio_fifo_write(resampler->audio_fifo, (void **)resampler->resampled_data, nb_samples);
}

int audio_resampler_send_frame3(audio_resampler_t *resampler, uint8_t *in_data, int in_bytes, int64_t pts)
{
    if(!resampler) {
        return 0;
    }

    AVFrame *frame = NULL;
    if(in_data)
    {
        frame = av_frame_alloc();
        frame->format = resampler->resampler_params.src_sample_fmt;
        frame->ch_layout = resampler->resampler_params.src_channel_layout;
        int ch = resampler->resampler_params.src_channel_layout.nb_channels;
        frame->nb_samples = in_bytes/ av_get_bytes_per_sample(resampler->resampler_params.src_sample_fmt) /ch;
        avcodec_fill_audio_frame(frame, ch, resampler->resampler_params.src_sample_fmt, in_data, in_bytes, 0);
        frame->pts = pts;
    }

    int ret = audio_resampler_send_frame(resampler, frame);
    if(frame) {
        av_frame_free(&frame);      // 释放内存
    }
    return ret;
}


int  audio_resampler_receive_frame2(audio_resampler_t *resampler, uint8_t **out_data, int nb_samples, int64_t *pts)
{
    nb_samples = nb_samples == 0 ? av_audio_fifo_size(resampler->audio_fifo) : nb_samples;
    if (av_audio_fifo_size(resampler->audio_fifo) < nb_samples || nb_samples == 0)
        return 0;
    int ret = av_audio_fifo_read(resampler->audio_fifo, (void **)out_data, nb_samples);
    *pts = resampler->out_cur_pts;
    resampler->out_cur_pts += nb_samples;
    resampler->total_resampled_num += nb_samples;
    return ret;
}


int audio_resampler_get_fifo_size(audio_resampler_t *resampler)
{
    if(!resampler) {
        return 0;
    }
    return av_audio_fifo_size(resampler->audio_fifo);   // 获取fifo的采样点数量
}

int64_t audio_resampler_get_start_pts(audio_resampler_t *resampler)
{
    if(!resampler) {
        return 0;
    }
    return resampler->out_start_pts;
}
int64_t audio_resampler_get_cur_pts(audio_resampler_t *resampler)
{
    if(!resampler) {
        return 0;
    }
    return resampler->out_cur_pts;
}


