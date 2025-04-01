## 基于FFmpeg开发的跨平台播放器,鸿蒙端

### 支持点播,直播服务
### 支持协议: HTTP/HTTPS, RTMP,RTSP,HLS
### 支持编解码:
    libx264(软) mp3lame(软) libfdk-aac(软)
    av1(软) hevc(软) av1(软)
### 支持封装格式: MP4 FLV MOV MKV TS AAC H264

### 基本支持:
    软硬解码切换
    暂停/恢复
    倍速播放,音量大小调整
    拖拽seekTo
    分段加载
    播放状态回调
    播放进度回调
    缓冲区时长回调