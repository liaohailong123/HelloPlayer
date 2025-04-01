package com.example.module_rtsp;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.Surface;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.Serializable;
import java.util.ArrayList;

/**
 * create by liaohailong
 * 2025/1/11 12:14
 * desc:
 */
public class HelloPlayer implements Serializable {
    static {
        System.loadLibrary("helloplayer");
    }

    private static void log(@NonNull String format, Object... args) {
        String msg = String.format(format, args);
        Log.i("HelloPlayer", msg);
    }

    private final long mPtr;

    /**
     * 所有控制相应，都通过 主线程 回调
     */
    protected final Handler mainHandler = new Handler(Looper.getMainLooper());

    // 回调接口
    private HelloPlayerCallback callback;

    // 缓存数据
    private HelloPlayerMediaInfo mediaInfo = null;

    private volatile HelloPlayerState currentState = HelloPlayerState.Idle;

    private final ArrayList<Surface> surfaces = new ArrayList<>();

    public HelloPlayer() {
        this.mPtr = nativeInit();
    }

    public void setConfig(@NonNull HelloPlayerConfig config) {
        nativeSetConfig(mPtr, config);
    }

    public HelloPlayerConfig getConfig() {
        return nativeGetConfig(mPtr);
    }

    /**
     * @return if player is prepared, media info is OK to return
     */
    @Nullable
    public HelloPlayerMediaInfo getMediaInfo() {
        return mediaInfo;
    }

    public long getDurationUs() {
        return mediaInfo != null ? mediaInfo.getDurationUs() : 0;
    }

    public void setDataSource(@NonNull String url) {
        nativeSetDataSource(mPtr, url);
    }

    public void addSurface(@NonNull Surface surface) {
        if (surfaces.contains(surface)) {
            return;
        }
        surfaces.add(surface);
        nativeAddSurface(mPtr, surface);
    }

    public void removeSurface(@NonNull Surface surface) {
        if (surfaces.contains(surface)) {
            nativeRemoveSurface(mPtr, surface);
        }
        surfaces.remove(surface);
    }

    public boolean isPrepared() {
        return currentState.compareTo(HelloPlayerState.Preparing) > 0;
    }

    public boolean isPlaying() {
        return currentState == HelloPlayerState.Playing;
    }

    public void prepare(long ptsUs) {
        nativePrepare(mPtr, ptsUs);
    }

    public void prepare() {
        nativePrepare(mPtr, 0);
    }

    public void start() {
        nativeStart(mPtr);
    }

    public void pause() {
        nativePause(mPtr);
    }

    public void seekTo(long ptsUs, boolean autoPlay) {
        nativeSeekTo(mPtr, ptsUs, autoPlay);
    }

    public void setVolume(double volume) {
        nativeSetVolume(mPtr, volume);
    }

    public void setSpeed(double speed) {
        nativeSetSpeed(mPtr, speed);
    }

    public void reset() {
        surfaces.clear();
        nativeReset(mPtr);
    }

    public void release() {
        nativeRelease(mPtr);
    }


    // set listeners
    public void setCallback(HelloPlayerCallback callback) {
        this.callback = callback;
    }

    protected native long nativeInit();

    protected native void nativeSetConfig(long ptr, @NonNull HelloPlayerConfig config);

    protected native HelloPlayerConfig nativeGetConfig(long ptr);

    protected native void nativeSetDataSource(long ptr, @NonNull String url);

    protected native void nativeAddSurface(long ptr, Surface surface);

    protected native void nativeRemoveSurface(long ptr, Surface surface);

    protected native void nativePrepare(long ptr, long ptsUs);

    protected native void nativeStart(long ptr);

    protected native void nativePause(long ptr);

    protected native void nativeSeekTo(long ptr, long ptsUs, boolean autoPlay);

    protected native void nativeSetVolume(long ptr, double volume);

    protected native void nativeSetSpeed(long ptr, double speed);

    protected native void nativeReset(long ptr);

    protected native void nativeRelease(long ptr);

    /**
     * Called by native
     *
     * @param info 多媒体信息
     */
    @Keep
    protected void onPrepared(@NonNull HelloPlayerMediaInfo info) {
//        log("onPrepared: \n%s", info.toString());
        mainHandler.post(() -> {
            currentState = HelloPlayerState.Prepared;
            mediaInfo = info;
            if (callback != null) callback.onPrepared(info);
        });
    }

    //    typedef enum
//    {
//        Idle = 0,
//        Preparing,
//        Prepared,
//        Playing,
//        Seeking,
//        Paused,
//
//        // 这个值用来记录有多少种播放状态
//        PlayStateNum
//    } PlayState;

    /**
     * Called by native
     *
     * @param from 播放状态(上次)回调
     * @param to   播放状态(本次)回调
     */
    @Keep
    protected void onPlayStateChanged(int from, int to) {
        mainHandler.post(() -> {
            HelloPlayerState fromState = HelloPlayerState.values()[from];
            HelloPlayerState toState = HelloPlayerState.values()[to];
            currentState = toState;
            if (callback != null) callback.onPlayStateChanged(fromState, toState);

            // 简单处理一下 缓冲区loading 的逻辑
            if (toState == HelloPlayerState.Preparing || toState == HelloPlayerState.Seeking) {
                if (callback != null) callback.onBufferLoadStart();
            } else if (toState == HelloPlayerState.Prepared || toState == HelloPlayerState.SeekEnd) {
                if (callback != null) callback.onBufferLoadEnd();
            }
        });
    }

    /**
     * Called by native
     *
     * @param error 错误码
     * @param msg   错误信息
     */
    @Keep
    protected void onErrorInfo(int error, String msg) {
        mainHandler.post(() -> {
            if (callback != null) {
                HelloPlayError target = HelloPlayError.UnKnown;
                for (HelloPlayError errorCode : HelloPlayError.values()) {
                    if (errorCode.code == error) {
                        target = errorCode;
                        break;
                    }
                }
                callback.onErrorInfo(target, msg);
            }
        });
    }

    /**
     * Called by native
     *
     * @param ptsUs      回调当前播放时间，单位：us
     * @param durationUs 总时长，单位：us
     */
    @Keep
    protected void onCurrentPosition(long ptsUs, long durationUs) {
        mainHandler.post(() -> {
            if (callback != null) callback.onCurrentPosition(ptsUs, durationUs);
        });
    }

    /**
     * Called by native
     *
     * @param startUs    缓冲区起始时间，单位：us
     * @param endUs      缓冲区结束时间，单位：us
     * @param durationUs 总时长，单位：us
     */
    @Keep
    protected void onBufferPosition(long startUs, long endUs, long durationUs) {
        mainHandler.post(() -> {
            if (callback != null) callback.onBufferPosition(startUs, endUs, durationUs);
        });
    }

}
