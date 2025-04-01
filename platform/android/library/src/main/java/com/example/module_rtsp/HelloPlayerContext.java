package com.example.module_rtsp;

import android.app.Application;

/**
 * Author: liaohailong
 * Date: 2024/12/19
 * Time: 17:06
 * Description:
 **/
public class HelloPlayerContext {

    private static class SingletonHolder {
        private static final HelloPlayerContext INSTANCE = new HelloPlayerContext();
    }

    public static HelloPlayerContext getInstance() {
        return SingletonHolder.INSTANCE;
    }


    private Application app;

    public void setApp(Application app) {
        this.app = app;
    }

    public Application getApp() {
        return app;
    }
}
