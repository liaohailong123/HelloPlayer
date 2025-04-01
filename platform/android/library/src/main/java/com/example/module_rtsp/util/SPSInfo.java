package com.example.module_rtsp.util;

import java.io.Serializable;

/**
 * create by liaohailong
 * 2024/12/15 11:54
 * desc:
 */
public class SPSInfo implements Serializable {

    public final int width;
    public final int height;

    public SPSInfo(int width, int height) {
        this.width = width;
        this.height = height;
    }
}
