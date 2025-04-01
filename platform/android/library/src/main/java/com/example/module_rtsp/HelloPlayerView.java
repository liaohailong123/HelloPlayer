package com.example.module_rtsp;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.SeekBar;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.example.module_rtsp.databinding.ViewHelloPlayerBinding;

import java.util.Locale;

/**
 * Author: liaohailong
 * Date: 2025/1/14
 * Time: 14:55
 * Description:
 **/
public class HelloPlayerView extends FrameLayout {
    private static void log(@NonNull String format, Object... args) {
        String msg = String.format(format, args);
        Log.i("HelloPlayerView", msg);
    }

    private static void toast(@NonNull String format, Object... args) {
        String msg = String.format(format, args);
        Toast.makeText(HelloPlayerContext.getInstance().getApp(), msg, Toast.LENGTH_SHORT).show();
    }

    private final ViewHelloPlayerBinding binding;

    @NonNull
    private final HelloPlayer player;

    private final HelloPlayerCallback callback = new HelloPlayerCallback() {
        /**
         * @param mediaInfo 播放器准备好后回调，多媒体信息
         */
        @Override
        public void onPrepared(@NonNull HelloPlayerMediaInfo mediaInfo) {
//            updateSurface(mediaInfo); // 逻辑调整:上层surface仅一份

            // 更新时长
            if (mediaInfo.isLiveStreaming) {
                binding.durationTxt.setText(formatMicrosToTime(0));
            } else {
                binding.durationTxt.setText(formatMicrosToTime(mediaInfo.getDurationUs()));
            }
            log("mediaInfo.isLiveStreaming[%s]", String.valueOf(mediaInfo.isLiveStreaming));


            if (surface != null) {
                binding.getRoot().post(new Runnable() {
                    @Override
                    public void run() {
                        player.addSurface(surface);
                    }
                });
            }
        }

        @Override
        public void onPlayStateChanged(@NonNull HelloPlayerState from, @NonNull HelloPlayerState to) {
            updatePlayOrPauseBtn(to == HelloPlayerState.Playing, false);
            log("onPlayStateChanged from %s to %s", from.name(), to.name());
        }

        @Override
        public void onErrorInfo(@NonNull HelloPlayError errorCode, @NonNull String message) {
            log("onErrorInfo code[%d:%s] msg[%s]", errorCode.code, errorCode.name(), message);
            toast(message);
        }

        @Override
        public void onBufferLoadStart() {
            binding.loadingContainer.setVisibility(View.VISIBLE);
            log("onBufferLoadStart");
        }

        @Override
        public void onBufferLoadEnd() {
            binding.loadingContainer.setVisibility(View.GONE);
            log("onBufferLoadEnd");
        }

        /**
         * @param ptsUs      当前主时钟播放时间，单位：us
         * @param durationUs 总时长
         */
        @Override
        public void onCurrentPosition(long ptsUs, long durationUs) {
//            log("onCurrentPosition ptsUs[%d] durationUs[%d]", ptsUs, durationUs);

            if (binding.seekBar.isSelected()) {
                // 用户正在手动拖拽seek bar
                return;
            }

            // 更新时长
            binding.currentTxt.setText(formatMicrosToTime(ptsUs));

            double progress = ptsUs * 1.0 / durationUs;
            int now = (int) Math.round(progress * 100);
            if (now > 100) now = 100;
//            log("seekBar.setProgress(%d)", now);
            binding.seekBar.setProgress(now);
        }


        /**
         * 当前缓冲区时间
         *
         * @param startUs 缓冲区起始时间，单位：us
         * @param endUs   缓冲区结束时间，单位：us
         * @param durationUs 总时长，单位：us
         */
        @Override
        public void onBufferPosition(long startUs, long endUs, long durationUs) {
//            log("onCurrentPosition startUs[%d] endUs[%d]", startUs, endUs);

            double progress = endUs * 1.0 / durationUs;
            int now = (int) Math.round(progress * 100);
            if (now > 100) now = 100;
//            log("seekBar.setSecondaryProgress(%d)", now);
            binding.seekBar.setSecondaryProgress(now);
        }
    };

    private Surface surface;

    public HelloPlayerView(@NonNull Context context) {
        this(context, null);
    }

    public HelloPlayerView(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public HelloPlayerView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        binding = ViewHelloPlayerBinding.inflate(LayoutInflater.from(context), this, true);

        player = new HelloPlayer();
        player.setCallback(callback);

        initSurfaceView();
        initSeekBar();
        initControllerView();
    }

    private void initSurfaceView() {
        binding.surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                log("surfaceCreated");
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                log("surfaceChanged size[%dx%d]", width, height);
                surface = holder.getSurface();
                binding.getRoot().post(new Runnable() {
                    @Override
                    public void run() {
                        if (player.isPrepared()) {
                            player.addSurface(surface);
                        }
                    }
                });

            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                log("surfaceDestroyed");
                player.removeSurface(holder.getSurface());
                surface = null;
            }
        });
    }

    private void initSeekBar() {
        binding.seekBar.setMax(100);
        binding.seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                // 更新时长
                double _progress = progress * 1.0 / seekBar.getMax();
                long ptsUs = (long) (player.getDurationUs() * _progress);
                binding.currentTxt.setText(formatMicrosToTime(ptsUs));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                seekBar.setSelected(true);
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                seekBar.setSelected(false);

                if (player.isPrepared()) {
                    int progress = seekBar.getProgress();
                    long durationUs = player.getDurationUs();
                    long ptsUs = (long) ((progress * 1.0 / 100.0) * durationUs);
                    log("seek to progress[%d] durationUs[%d]", progress, durationUs);
                    boolean autoPlay = player.isPlaying();
//                    ptsUs = 100 * 1000 * 1000; // 测试：固定时间
                    player.seekTo(ptsUs, autoPlay);
                    log("seek to pts[%d]ms autoplay[%s]", ptsUs / 1000, String.valueOf(autoPlay));
                }

            }
        });
    }

    private void initControllerView() {
        updatePlayOrPauseBtn(false, false);
        binding.playOrPauseBtn.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean play = v.isSelected();
                updatePlayOrPauseBtn(!play, true);
            }
        });
    }

    public static String formatMicrosToTime(long micros) {
        long totalSeconds = micros / 1_000_000; // 将微秒转换为秒
        long hours = totalSeconds / 3600;       // 小时部分
        long minutes = (totalSeconds % 3600) / 60;  // 分钟部分
        long seconds = totalSeconds % 60;      // 秒数部分

        if (hours > 0) {
            return String.format(Locale.getDefault(), "%02d:%02d:%02d", hours, minutes, seconds);
        } else {
            return String.format(Locale.getDefault(), "%02d:%02d", minutes, seconds);
        }
    }


    private void updatePlayOrPauseBtn(boolean play, boolean action) {
        binding.playOrPauseBtn.setText(play ? "暂停" : "播放");
        binding.playOrPauseBtn.setSelected(play);
        if (action) {
            if (play) {
                player.start();
            } else {
                player.pause();
            }
        }
    }

    private void updateSurface(@NonNull HelloPlayerMediaInfo info) {
        log("update surface size[%dx%d]", info.width, info.height);
        // 调整画面尺寸，自适应屏幕尺寸，保证视频渲染比例
        binding.surfaceView.setAspectRatio(info.width, info.height);
    }

    public void setPlayerConfig(@NonNull HelloPlayerConfig config) {
        player.setConfig(config);
    }

    @Nullable
    public HelloPlayerConfig getPlayerConfig() {
        return player.getConfig();
    }

    public void setDataSource(@NonNull String url) {
        player.setDataSource(url);
    }

    public void prepare(long ptsUs) {
        player.prepare(ptsUs);
    }

    public boolean isPrepared() {
        return player.isPrepared();
    }

    public void start() {
        player.start();
    }

    public void pause() {
        player.pause();
    }

    public void seekTo(long ptsUs, boolean autoPlay) {
        player.seekTo(ptsUs, autoPlay);
    }

    public void setVolume(double volume) {
        if (player.isPrepared()) {
            player.setVolume(volume);
        }
    }

    public void setSpeed(double speed) {
        if (player.isPrepared()) {
            player.setSpeed(speed);
        }
    }

    public void reset() {
        player.reset();
    }

    public void release() {
        player.release();
    }
}
