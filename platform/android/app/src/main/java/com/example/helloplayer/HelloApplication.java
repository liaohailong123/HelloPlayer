package com.example.helloplayer;

import android.app.Application;

import com.example.module_rtsp.HelloPlayerContext;

/**
 * create by liaohailong
 * 2025/1/11 14:15
 * desc:
 */
public class HelloApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        HelloPlayerContext.getInstance().setApp(this);
    }
}
