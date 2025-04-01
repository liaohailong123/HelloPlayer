package com.example.module_rtsp.entity;

import android.media.AudioFormat;
import android.media.MediaFormat;

import androidx.annotation.NonNull;

import java.io.Serializable;

/**
 * Author: liaohailong
 * Date: 2024/11/6
 * Time: 19:51
 * Description: 音频编码配置信息
 **/
public class MAudioProperties implements Serializable {

    /**
     * 采样率
     */
    public int sampleRate = 44100;
    /**
     * 声道数
     */
    public int channelCount = AudioFormat.CHANNEL_IN_MONO;
    /**
     * 位深类型
     */
    public int sampleFmt = AudioFormat.ENCODING_PCM_16BIT;

    /**
     * 比特率，直接影响音频编码数据大小
     */
    public int bitrate = 196000;

    /**
     * 音频编码格式
     */
    public String mimeType = MediaFormat.MIMETYPE_AUDIO_AAC;

    @NonNull
    @Override
    public String toString() {
        return "MAudioProperties{" +
                "sampleRate=" + sampleRate +
                ", channelCount=" + channelCount +
                ", sampleFmt=" + sampleFmt +
                ", bitrate=" + bitrate +
                ", mimeType='" + mimeType + '\'' +
                '}';
    }
}
