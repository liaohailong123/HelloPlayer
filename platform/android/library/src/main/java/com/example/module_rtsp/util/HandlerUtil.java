package com.example.module_rtsp.util;

import android.os.Handler;
import android.os.HandlerThread;

import androidx.annotation.NonNull;

/**
 * Author: liaohailong
 * Date: 2024/11/8
 * Time: 17:08
 * Description:
 **/
public class HandlerUtil {
    public static Handler newThreadHandler(@NonNull String name) {
        HandlerThread ht = new HandlerThread(name);
        ht.start();
        return new Handler(ht.getLooper());
    }
}
