# 一款基于FFmpeg封装的移动端播放器，支持Android/鸿蒙/iOS


<img src="https://github.com/liaohailong123/HelloPlayer/blob/master/assets/Screenrecorder-2025-02-07-14-09-15-263.gif" alt="科学上网 或 Github 加速器" width="224px">


## 先运行 python init.py 初始化工作，然后再进入 platform 对应平台运行示例工程

## 第三方库依赖：
	ffmpeg version = ffmpeg-7.0.1
	libx264-master libdav1d-master libfdk-aac-2.0.3 libmp3lame-3.100 openssl-3.3.1
	
	Android平台：
		ndk version = 21.4.7075529 minSdk=21
	鸿蒙平台：
		ndk = openharmony/native 鸿蒙Next版本
	iOS平台
		iPhoneOS 8.0 + iPhoneSimulator 8.0

## 工程目录
	3rdpart 音视频第三方库
	framework 跨平台播放器内核代码
	platform 移动端平台侧代码 + 示例工程
		android: 		app(示例工程) + library(平台侧代码)
		ohos(鸿蒙Next): 	entry(示例工程) + helloplayer(平台侧代码)
		ios：			example(示例工程) + helloplayer(平台侧代码)

## 音频播放
	Android: Obo库(OpenSLES/AAudio)
	鸿蒙：native_audiorender
	iOS: AudioUnit
	
## 视频渲染
	Android: SurfaceView + OpenGLES
	鸿蒙：XComponent + OpenGLES
	iOS: 
		方式一：CAEAGLLayer + OpenGLES
		方式二：MTKView + Metal
		
## 音视频解码
	音频：全平台 ffmpeg 软解
	视频：
		全平台 ffmpeg 软解
		Android MediaCodec硬解
		Ohos(鸿蒙) OH_AVCodec硬解
		iOS Videotoolbox硬解

## 功能      
    支持点播,直播服务
    支持协议: HTTP/HTTPS, RTMP,RTSP,HLS
    支持编解码: 
		ffmpeg所有软解码 + libx264 libdav1d mp3lame libfdk-aac
		Android硬解码：h264 hevc av1 mpeg2 mpeg4 vp8 vp9
		Ohos硬解码：h264 hevc vvc
		iOS硬解码：h263 h264 hevc mpeg1 mpeg2 mpeg4 vp9

    支持封装格式: ffmpeg中所有封装格式，常见的：MP4 FLV MOV MKV H264 TS M3U8 ...

    基本支持:
        软硬解码切换
        暂停/恢复
        倍速播放,音量大小调整
        拖拽seekTo
        分段加载
		播放状态回调
		播放进度回调
		缓冲区时长回调

## 示例代码

```xml
<com.example.module_rtsp.HelloPlayerView
            android:id="@+id/player_view"
            android:layout_width="match_parent"
            android:layout_height="wrap_content"
            android:layout_gravity="center" />
```

```java
    HelloPlayerConfig config = new HelloPlayerConfig(); // 创建播放器配置属性
    // 添加配置
    config.bufferDurationUs = 10 * 1000 * 1000; // 合理设置，不用太大
    config.autoPlay = true; // 自动起播
    config.loopPlay = true; // 循环播放
    config.speed = 1.0; // 播放速率，不宜设置过高，视频解码器跟不上
    config.volume = 1.0; // 音量大小，不宜设置过高 0到10的范围效果明显
    config.useMediaCodec = false; // 是否采用硬解码

    // 配置播放器
    if (playerView.isPrepared()) {
        playerView.reset();
    }
    
    playerView.setPlayerConfig(config);
    playerView.setDataSource("Your uri");
    playerView.prepare(startOffsetUs); // 准备,可设置起播时间
```


## 类图


![1](https://github.com/liaohailong123/HelloPlayer/blob/master/assets/HelloPlayer.svg)



