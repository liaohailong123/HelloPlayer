package com.example.module_rtsp.util;

/**
 * create by liaohailong
 * 2024/12/16 22:27
 * desc: SpeexDSP库，降噪和去回声有前提条件：
 * 1，位深必须16位整型
 * 2，声道数必须单声道
 * 3，采样率建议 8K 或16K，44100赫兹也支持，可能性能损耗比较大
 */
public class SpeexDSP {

    private final long mPtr;

    /**
     * 使用Speex进行降噪去回声
     *
     * @param frameSize    单声道样本数量
     * @param sampleRate   采样率
     * @param filterLength 回声消除滤波器长度
     */
    public SpeexDSP(int frameSize, int sampleRate, int filterLength) {
        this.mPtr = nativeInit(frameSize, sampleRate, filterLength);
    }

    /**
     * 预处理音频数据（降噪+去回声）
     *
     * @param playback   播放器刚刚播放的PCM数据
     * @param capture    麦克风刚刚收到的PCM数据
     * @param frame_size 样本数量
     * @return 降噪+去回声 的PCM数据
     */
    public byte[] preprocess(byte[] playback, byte[] capture, int frame_size) {
        // 将 byte[] 数组转换为 short[] 数组
        short[] playbackShort = new short[frame_size];
        short[] captureShort = new short[frame_size];

        // 将 playback byte[] 转换为 short[]
        for (int i = 0; i < frame_size; i++) {
            playbackShort[i] = (short) ((playback[2 * i] & 0xFF) | (playback[2 * i + 1] << 8));
        }

        // 将 capture byte[] 转换为 short[]
        for (int i = 0; i < frame_size; i++) {
            captureShort[i] = (short) ((capture[2 * i] & 0xFF) | (capture[2 * i + 1] << 8));
        }

        // 处理回声消除和降噪（调用 JNI 方法）
        short[] output = preprocess(playbackShort, captureShort, frame_size);

        // 将处理后的 short[] 转换回 byte[]
        byte[] outputByte = new byte[frame_size * 2];
        for (int i = 0; i < frame_size; i++) {
            outputByte[2 * i] = (byte) (output[i] & 0xFF);
            outputByte[2 * i + 1] = (byte) ((output[i] >> 8) & 0xFF);
        }

        return outputByte;

    }

    /**
     * 预处理音频数据（降噪+去回声）
     *
     * @param playback   播放器刚刚播放的PCM数据
     * @param capture    麦克风刚刚收到的PCM数据
     * @param frame_size 样本数量
     * @return 降噪+去回声 的PCM数据
     */
    public short[] preprocess(short[] playback, short[] capture, int frame_size) {
        return nativePreprocess(mPtr, playback, capture, frame_size);
    }

    public void release() {
        nativeRelease(mPtr);
    }

    protected native long nativeInit(int frame_size, int sample_rate, int filter_length);

    protected native short[] nativePreprocess(long ptr, short[] playback, short[] capture, int frame_size);

    protected native void nativeRelease(long ptr);
}
