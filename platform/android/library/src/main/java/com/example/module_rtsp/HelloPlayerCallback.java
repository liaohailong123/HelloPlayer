package com.example.module_rtsp;

import androidx.annotation.NonNull;

/**
 * Author: liaohailong
 * Date: 2025/1/14
 * Time: 14:45
 * Description:
 **/
public interface HelloPlayerCallback {

    /**
     * @param mediaInfo 播放器准备好后回调，多媒体信息
     */
    void onPrepared(@NonNull HelloPlayerMediaInfo mediaInfo);


    /**
     * @param from 播放状态(上次)回调
     * @param to 播放状态(本次)回调
     */
    void onPlayStateChanged(@NonNull HelloPlayerState from, @NonNull HelloPlayerState to);

    /**
     * @param errorCode 错误码
     * @param message 错误信息
     */
    void onErrorInfo(@NonNull HelloPlayError errorCode, @NonNull String message);

    /**
     * 数据加载开始
     */
    void onBufferLoadStart();

    /**
     * 数据加载结束
     */
    void onBufferLoadEnd();

    /**
     * @param ptsUs      当前主时钟播放时间，单位：us
     * @param durationUs 总时长，单位：us
     */
    void onCurrentPosition(long ptsUs, long durationUs);

    /**
     * 当前缓冲区时间
     *
     * @param startUs 缓冲区起始时间，单位：us
     * @param endUs   缓冲区结束时间，单位：us
     * @param durationUs 总时长，单位：us
     */
    void onBufferPosition(long startUs, long endUs, long durationUs);
}
