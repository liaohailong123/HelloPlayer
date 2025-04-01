package com.example.module_rtsp.util;


import androidx.annotation.WorkerThread;

import java.io.Serializable;
import java.nio.ByteBuffer;

/**
 * Author: liaohailong
 * Date: 2024/12/6
 * Time: 10:57
 * Description:
 **/
public class H264SPSParser implements Serializable {

    public static boolean isSPS(byte[] buffer) {
        if (buffer == null || buffer.length < 1) return false;
        return (buffer[0] & 0x1F) == 7;
    }

    public static boolean isPPS(byte[] buffer) {
        if (buffer == null || buffer.length < 1) return false;
        return (buffer[0] & 0x1F) == 8;
    }

    public static boolean isIDR(byte[] buffer) {
        if (buffer == null || buffer.length < 1) return false;
        return (buffer[0] & 0x1F) == 5;
    }

    public static boolean isIFrame(byte[] buffer) {
        if (buffer == null || buffer.length < 1) return false;
        return (buffer[0] & 0x61) == 97;
    }

    public static boolean isPFrame(byte[] buffer) {
        if (buffer == null || buffer.length < 1) return false;
        return (buffer[0] & 0x61) == 65;
    }

    public static boolean isBFrame(byte[] buffer) {
        if (buffer == null || buffer.length < 1) return false;
        return (buffer[0] & 0x61) == 1;
    }

    public static boolean hasStartCode(byte[] buffer) {
        return startCode3(buffer) || startCode4(buffer);
    }

    public static boolean startCode3(byte[] buffer) {
        if (buffer.length < 3) return false;
        return buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 1;
    }

    public static boolean startCode4(byte[] buffer) {
        if (buffer.length < 4) return false;
        return buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0 && buffer[3] == 1;
    }


    /**
     * 解析 SPS 数据获取视频分辨率
     *
     * @param spsBuffer 包含 SPS 数据的 ByteBuffer
     * @return 视频分辨率 (width, height)，如果解析失败返回 null
     */
    @WorkerThread
    public static SPSInfo parseSPS(byte[] spsBuffer) {
        int pos = 0;
        if (startCode3(spsBuffer)) {
            pos += 3;
        } else if (startCode4(spsBuffer)) {
            pos += 4;
        }
        byte[] bytes = new byte[spsBuffer.length - pos];
        System.arraycopy(spsBuffer, 0, bytes, pos, bytes.length);
        return nativeParseSPS(bytes);
    }

    /**
     * SPS数据解析，需要去掉 startCode： 00 00 01 或 00 00 00 01
     *
     * @param sps SPS数据内容，需要Nal类型为0x7数据的开始(比如：67 42 00 28 ab 40 22 01 e3 cb cd c0 80 80 a9 02)
     * @return 解析结果
     */
    @WorkerThread
    protected static native SPSInfo nativeParseSPS(byte[] sps);

}
