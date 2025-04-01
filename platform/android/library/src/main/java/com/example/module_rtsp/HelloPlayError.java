package com.example.module_rtsp;

/**
 * create by liaohailong
 * 2025/2/9 9:39
 * desc: 对应 HelloPlayerError.hpp
 */
public enum HelloPlayError {
    UnKnown(-1), // 未知错误，底层上报的错误，应用上层对应不上

    None(0), // 无错误，表示成功

    NoDataSource(-100), // 资源没有设置
    NoStreamInDataSource(-101), // 资源中没有任何轨道信息
    NoAudioVideoStream(-102), // 资源中没有音视频轨道

    NetworkTimeout(-400), // 网络连接超时


    ;

    public final int code;

    HelloPlayError(int code) {
        this.code = code;
    }
}
