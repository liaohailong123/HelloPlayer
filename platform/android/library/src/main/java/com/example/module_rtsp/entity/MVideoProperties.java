package com.example.module_rtsp.entity;

import android.app.Activity;
import android.content.res.Configuration;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;

import androidx.annotation.NonNull;

import java.io.Serializable;
import java.lang.ref.WeakReference;

/**
 * Author: liaohailong
 * Date: 2024/11/6
 * Time: 19:52
 * Description: 视频编码配置信息
 **/
public class MVideoProperties implements Serializable {
    private final WeakReference<Activity> activityRef;
    /**
     * 视频编码类型
     */
    public String mimeType;
    /**
     * 视频宽
     */
    public int width;
    /**
     * 视频高
     */
    public int height;

    /**
     * 帧率
     */
    public int fps;
    /**
     * 比特率
     */
    public int bitrate;
    /**
     * 几秒钟一个I帧
     */
    public int iFrameInternal;
    /**
     * 编码源数据类型 Media参考：MediaCodecInfo.CodecCapabilities 类
     */
    public int colorFormat;

    /**
     * csd-0 存放SPS
     */
    public byte[] csd_0;
    /**
     * csd-1 存放PPS
     */
    public byte[] csd_1;


    public MVideoProperties(Activity activity) {
        this.activityRef = new WeakReference<>(activity);
    }

    /**
     * 获取预览信息
     *
     * @return int[0]=width int[1]=height int[2]=rotation
     */
    public int[] getDisplayInfo() {
        Activity activity = activityRef.get();
        int[] result = new int[3];
        result[0] = width;
        result[1] = height;

        int orientation = Configuration.ORIENTATION_PORTRAIT;
        if (activity != null) {
            orientation = activity.getResources().getConfiguration().orientation;
        }
        // 横竖屏可能和相机的渲染尺寸有冲突，手动调整一下
        if (Configuration.ORIENTATION_PORTRAIT == orientation) {
            // 竖屏模式，如果宽大于高，替换一下尺寸，然后标记旋转90度
            result[0] = Math.min(width, height);
            result[1] = Math.max(width, height);
            result[2] = width > height ? 90 : 0;
        } else {
            // 横屏模式，如果宽小高，替换一下尺寸，然后标记旋转90度
            result[0] = Math.max(width, height);
            result[1] = Math.min(width, height);
            result[2] = width > height ? 0 : 90;
        }
        return result;
    }

    @NonNull
    @Override
    public String toString() {
        return "MVideoProperties{" +
                "mimeType='" + mimeType + '\'' +
                ", width=" + width +
                ", height=" + height +
                ", fps=" + fps +
                ", bitrate=" + bitrate +
                ", iFrameInternal=" + iFrameInternal +
                ", colorFormat=" + colorFormat +
                '}';
    }
}
