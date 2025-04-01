package com.example.helloplayer;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.SeekBar;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.example.helloplayer.databinding.ActivityMainBinding;
import com.example.helloplayer.util.FileUtils;
import com.example.module_rtsp.HelloPlayerConfig;

import java.io.File;
import java.util.ArrayList;

/**
 * create by liaohailong
 * 2024/12/23 21:01
 * desc:
 */
public class MainActivity extends AppCompatActivity {
    private static void log(@NonNull String format, Object... args) {
        String msg = String.format(format, args);
        Log.i("Victor", msg);
    }

    private ActivityMainBinding binding;

    private final HelloPlayerConfig config = new HelloPlayerConfig();
    private String destPath;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // 屏幕常量
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // 把数据拷贝到硬盘上
        String dirName = "mp4";
        File dir = getExternalFilesDir(dirName);
        assert (dir != null);
        destPath = dir.getAbsolutePath();
        new Thread(new Runnable() {

            @Override
            public void run() {
                String[] list = new File(destPath).list();
                if (list == null || list.length == 0) {
                    FileUtils.copyAssets(MainActivity.this, dirName, destPath);
                    log("copy assets success");
                }
            }
        }).start();

        // 默认配置
        config.bufferDurationUs = 10 * 1000 * 1000; // 合理设置，不用太大
        config.autoPlay = true; // 自动起播
        config.loopPlay = true; // 循环播放
        config.speed = 1.0; // 播放速率，不宜设置过高，视频解码器跟不上
        config.volume = 1.0; // 音量大小，不宜设置过高 0到10的范围效果明显
        updateSpeedBtn(binding.speedNormalBtn);

        long startOffsetUs = 0;
        // Sync-One2_Test_1080p_23.98_H.264_AAC_5.1.mov
        // Sync-One2_Test_1080p_23.98_H.264_AAC_5.1.mp4

        String[] urls = {
                new File(destPath, "Sync-One2_Test_1080p_23.98_H.264_AAC_5.1.mp4").getAbsolutePath(),
                new File(destPath, "output_av1.mp4").getAbsolutePath(),
                // h265
                "http://demo-videos.qnsdk.com/bbk-H265-50fps.mp4",
                // 线上
                "http://demo-videos.qnsdk.com/VR-Panorama-Equirect-Angular-4500k.mp4",
                // 吃鸡直播
                "rtmp://liteavapp.qcloud.com/live/liteavdemoplayerstreamid",
        };

        // 软解播放 h264
        binding.prepareSoftBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (binding.playerView.isPrepared()) {
                    binding.playerView.reset();
                }
                config.useHardware = false; // 是否采用硬解码
                binding.playerView.setPlayerConfig(config);
                binding.playerView.setDataSource(urls[0]);
                binding.playerView.prepare(startOffsetUs);
                updateCodecBtn(v);
            }
        });

        // 软解播放 av1
        binding.prepareSoft2Btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (binding.playerView.isPrepared()) {
                    binding.playerView.reset();
                }
                config.useHardware = false; // 是否采用硬解码
                binding.playerView.setPlayerConfig(config);
                binding.playerView.setDataSource(urls[1]);
                binding.playerView.prepare(startOffsetUs);
                updateCodecBtn(v);
            }
        });

        // 软解播放 h265
        binding.prepareSoft3Btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (binding.playerView.isPrepared()) {
                    binding.playerView.reset();
                }
                config.useHardware = false; // 是否采用硬解码
                binding.playerView.setPlayerConfig(config);
                binding.playerView.setDataSource(urls[2]);
                binding.playerView.prepare(startOffsetUs);
                updateCodecBtn(v);
            }
        });

        // 硬解播放 h264 网络
        binding.prepareHardBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (binding.playerView.isPrepared()) {
                    binding.playerView.reset();
                }
                config.useHardware = true; // 是否采用硬解码
                binding.playerView.setPlayerConfig(config);
                binding.playerView.setDataSource(urls[3]);
                binding.playerView.prepare(startOffsetUs);
                updateCodecBtn(v);
            }
        });

        // 硬解播放 av1
        binding.prepareHard2Btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (binding.playerView.isPrepared()) {
                    binding.playerView.reset();
                }
                config.useHardware = true; // 是否采用硬解码
                updateSpeedBtn(binding.speedNormalBtn);
                binding.playerView.setPlayerConfig(config);
                binding.playerView.setDataSource(urls[1]);
                binding.playerView.prepare(startOffsetUs);
                updateCodecBtn(v);
            }
        });

        // 硬解播放 h265
        binding.prepareHard3Btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (binding.playerView.isPrepared()) {
                    binding.playerView.reset();
                }
                config.useHardware = true; // 是否采用硬解码
                config.speed = 1.0; // 直播不变速
                updateSpeedBtn(binding.speedNormalBtn);
                binding.playerView.setPlayerConfig(config);
                binding.playerView.setDataSource(urls[2]);
                binding.playerView.prepare(startOffsetUs);
                updateCodecBtn(v);
            }
        });

        // 硬解播放 h264 直播
        binding.prepareHard4Btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (binding.playerView.isPrepared()) {
                    binding.playerView.reset();
                }
                config.useHardware = true; // 是否采用硬解码
                config.speed = 1.0; // 直播不变速
                updateSpeedBtn(binding.speedNormalBtn);
                binding.playerView.setPlayerConfig(config);
                binding.playerView.setDataSource(urls[4]);
                binding.playerView.prepare(startOffsetUs);
                updateCodecBtn(v);
            }
        });

        bindOptionListener();
    }

    private void bindOptionListener() {
        // 播放速度
        binding.speedLowBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                binding.playerView.setSpeed(0.5);
                config.speed = 0.5; // 播放速率，不宜设置过高，视频解码器跟不上
                updateSpeedBtn(binding.speedLowBtn);
            }
        });
        binding.speedNormalBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                binding.playerView.setSpeed(1.0);
                config.speed = 1.0; // 播放速率，不宜设置过高，视频解码器跟不上
                updateSpeedBtn(binding.speedNormalBtn);
            }
        });
        binding.speedFast1Btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                binding.playerView.setSpeed(1.5);
                config.speed = 1.5; // 播放速率，不宜设置过高，视频解码器跟不上
                updateSpeedBtn(binding.speedFast1Btn);
            }
        });
        binding.speedFast2Btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                binding.playerView.setSpeed(2.0);
                config.speed = 2.0; // 播放速率，不宜设置过高，视频解码器跟不上
                updateSpeedBtn(binding.speedFast2Btn);
            }
        });
        binding.speedFast3Btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                binding.playerView.setSpeed(3.0);
                config.speed = 3.0; // 播放速率，不宜设置过高，视频解码器跟不上
                updateSpeedBtn(binding.speedFast3Btn);
            }
        });

        // 音量大小
        binding.volumeSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                double volume = progress * 1.0 / seekBar.getMax();
                volume *= 2;
                binding.playerView.setVolume(volume);
                log("set volume[%f]", volume);
                binding.volumeTxt.setText(String.valueOf(progress));
                config.volume = volume;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    private void updateCodecBtn(@NonNull View target) {
        ArrayList<View> views = new ArrayList<>();
        int count = binding.prepareContainer.getChildCount();
        for (int i = 0; i < count; i++) {
            views.add(binding.prepareContainer.getChildAt(i));
        }

        for (View view : views) {
            view.setSelected(view == target);
        }

    }

    private void updateSpeedBtn(@NonNull View target) {
        ArrayList<View> views = new ArrayList<>();
        int count = binding.speedContainer.getChildCount();
        for (int i = 0; i < count; i++) {
            views.add(binding.speedContainer.getChildAt(i));
        }

        for (View view : views) {
            view.setSelected(view == target);
        }
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        binding.playerView.release();
    }
}
