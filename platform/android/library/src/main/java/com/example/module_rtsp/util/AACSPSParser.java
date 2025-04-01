package com.example.module_rtsp.util;

/**
 * Author: liaohailong
 * Date: 2024/12/6
 * Time: 13:55
 * Description:
 **/
public class AACSPSParser {
    /**
     * 生成 AAC 的 AudioSpecificConfig 数据
     *
     * @param sampleRate   采样率，例如 44100, 48000
     * @param channelCount 声道数，例如 1（单声道）, 2（立体声）
     * @return 返回包含 AudioSpecificConfig 的字节数组
     */
    public static byte[] createAACCodecSpecificData(int sampleRate, int channelCount) {
        // 获取采样率对应的索引值
        int sampleRateIndex = getAACSampleRateIndex(sampleRate);
        if (sampleRateIndex < 0) {
            throw new IllegalArgumentException("Unsupported sample rate: " + sampleRate);
        }

        // 声道配置
        int channelConfig = channelCount;

        // 创建 AudioSpecificConfig，AAC-LC 的 object type 固定为 2
        byte[] config = new byte[2];
        config[0] = (byte) ((0x2 << 3) | (sampleRateIndex >> 1));  // object type (AAC-LC) + Sampling Frequency Index (高 3 bits)
        config[1] = (byte) (((sampleRateIndex & 0x1) << 7) | (channelConfig << 3));  // Sampling Frequency Index (低 1 bit) + Channel Configuration

        return config;
    }

    /**
     * 获取 AAC 采样率的索引
     *
     * @param sampleRate 采样率，例如 44100, 48000
     * @return 返回采样率对应的索引值
     */
    private static int getAACSampleRateIndex(int sampleRate) {
        // 定义 AAC 支持的采样率及其对应索引
        int[] sampleRates = {
                96000, 88200, 64000, 48000, 44100, 32000,
                24000, 22050, 16000, 12000, 11025, 8000, 7350
        };
        for (int i = 0; i < sampleRates.length; i++) {
            if (sampleRates[i] == sampleRate) {
                return i;
            }
        }
        return -1; // 未找到对应的采样率
    }

    // 添加 ADTS 头
    public static void add_adts_header(byte[] header, int packet_len, int sample_rate, int channels) {
        int profile = 1; // AAC LC
        int freq_idx = 4; // 默认采样率 44100 Hz（0: 96000, 4: 44100）
        switch (sample_rate) {
            case 96000:
                freq_idx = 0;
                break;
            case 88200:
                freq_idx = 1;
                break;
            case 64000:
                freq_idx = 2;
                break;
            case 48000:
                freq_idx = 3;
                break;
            case 44100:
                freq_idx = 4;
                break;
            case 32000:
                freq_idx = 5;
                break;
            case 24000:
                freq_idx = 6;
                break;
            case 22050:
                freq_idx = 7;
                break;
            case 16000:
                freq_idx = 8;
                break;
            case 12000:
                freq_idx = 9;
                break;
            case 11025:
                freq_idx = 10;
                break;
            case 8000:
                freq_idx = 11;
                break;
            default:
                freq_idx = 4;
                break; // 默认 44100
        }

        int chan_cfg = channels; // 声道数
        int adts_len = packet_len + 7;


        header[0] = (byte) 0xFF; // 同步字
        header[1] = (byte) 0xF1; // 同步字 + MPEG-4
        header[2] = (byte) ((profile << 6) + (freq_idx << 2) + (chan_cfg >> 2));
        header[3] = (byte) (((chan_cfg & 3) << 6) + (adts_len >> 11));
        header[4] = (byte) ((adts_len & 0x7FF) >> 3);
        header[5] = (byte) (((adts_len & 7) << 5) + 0x1F);
        header[6] = (byte) 0xFC;
    }
}
