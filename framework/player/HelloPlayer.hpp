//
// Created by liaohailong on 2024/12/19.
//

#ifndef HELLOPLAYER_HELLOPLAYER_HPP
#define HELLOPLAYER_HELLOPLAYER_HPP

// 播放器重要组件部分
#include "component/HelloDataReader.hpp"
#include "component/HelloAVDecoder.hpp"
#include "component/HelloAudioPlayer.hpp"
#include "component/HelloVideoPlayer.hpp"

// 时钟与音视频同步
#include "HelloAVSync.hpp"

// 配置与属性
#include "entity/PlayConfig.hpp"
#include "entity/MediaInfo.hpp"
#include "HelloPlayerError.hpp"

// 回调相关
#include "HelloPlayerCallback.hpp"

// 系统库
#include <memory>

/**
 * create by liaohailong
 * 2024/12/19 21:32
 * desc: 
 */
class HelloPlayer
{
public:
    typedef enum
    {
        Idle = 0,
        Preparing,
        Prepared,
        Playing,
        Seeking,
        SeekEnd,
        Paused,
        Complete,

        // 这个值用来记录有多少种播放状态
        PlayStateNum
    } PlayState;

    typedef struct
    {
        int64_t seekToUs;
        bool autoPlay;
    } SeekAction;
public:
    explicit HelloPlayer(void* userdata = nullptr);

    ~HelloPlayer();

    void setConfig(PlayConfig config);

    PlayConfig getConfig();

    void setDataSource(const char *uri);

    void addSurface(void *surface);

    void removeSurface(void *surface);

    void prepare(int64_t ptsUs = 0);

    void start();

    bool isPrepared();

    bool isPlaying();

    bool isSeeking();

    void seekTo(int64_t ptsUs, bool autoPlay = true);

    void pause();

    /**
     * 调整音量大小
     * @param volume 0.0=静音 1.0=原始音量 大于1.0=扩大音量
     */
    void setVolume(double volume);

    /**
     * 调整播放速率
     * @param speed [0.5,100.0]
     */
    void setSpeed(double speed);

    /**
     * 进入空闲状态，可重新设置资源调用prepare
     */
    void reset();


private: // Handler回调接口
    static void registerAll(void *userData);

    /**
     * 开始准备加载资源
     * @param userData userData 用户携带参数
     */
    static void doPrepare(void *userData);

    /**
     * 开始播放
     * @param userData 用户携带参数
     */
    static void doStart(void *userData);

    /**
     * 暂停播放
     * @param userData 用户携带参数
     */
    static void doPause(void *userData);

    /**
     * 跳转位置
     * @param userData 用户携带参数
     */
    static void doSeek(void *userData);

    /**
     * 跳转操作结束，此时已经有数据加载出来了
     * @param userData  用户携带参数
     */
    static void onSeekEnd(void *userData);

    /**
     * 音视频编码数据加载回调，one by one
     * @param output 解复用拿出来的编码数据包
     * @param userdata 用户携带参数
     */
    static void onPacketRead(const std::shared_ptr<IAVPacket> &output, void *userdata);

    /**
     * 音频编码数据 解码成功后回调
     * @param output 解码好的数据
     * @param userdata 用户携带参数
     */
    static void onAudioDecoded(const std::shared_ptr<IAVFrame> &output, void *userdata);

    /**
     * 视频编码数据 解码成功后回调
     * @param output 解码好的数据
     * @param userdata 用户携带参数
     */
    static void onVideoDecoded(const std::shared_ptr<IAVFrame> &output, void *userdata);
    /**
     * 音频数据播放完毕回调
     * @param output 播放完的数据
     * @param userdata 用户携带参数
     */
    static void onAudioRendered(const std::shared_ptr<IAVFrame> &output, void *userdata);
    /**
     * 视频数据播放完毕回调
     * @param output 播放完的数据
     * @param userdata 用户携带参数
     */
    static void onVideoRendered(const std::shared_ptr<IAVFrame> &output, void *userdata);

    /**
     * 主时钟更新回调
     * @param ptsUs 当前更新时间回调（变速后），单位：us
     * @param userdata 用户携带参数
     */
    static void onMasterClockUpdate(int64_t ptsUs, void *userdata);

    /**
     * 重新恢复到空闲状态，随后可重新设置资源调用prepare
     * @param userdata 用户携带参数
     */
    static void onPlayerReset(void *userdata);

private: // 内部逻辑

    /**
     * 打开资源，准备进入prepare状态
     */
    bool openSource();

    /**
     * 更新当前播放器状态
     * @param state 待更新的状态
     * @return true表示更新成功，false表示更新失败
     */
    bool updateState(PlayState state);

    /**
     * 开始加载下一段播放数据
     * @param seekOnce 本次操作需要偏移一次数据读取的位置
     * @return true表示还能继续加载数据
     */
    bool loadNextPlayRange(bool seekOnce = false);

    /**
     * @return 获取下一段播放范围
     */
    std::shared_ptr<PlayRange> getNextPlayRange();

    /**
     * @param frame 更新缓冲区范围
     */
    void updateBufferRange(const std::shared_ptr<IAVFrame> &frame);

    /**
     * 播放完毕后的处理逻辑
     */
    void onPlayComplete();

private:
    Logger logger;

    /**
     * 数据回调
     */
    std::shared_ptr<HelloPlayerCallback> callback;

    /**
     * 播放配置属性
     */
    PlayConfig playConfig;

    /**
     * 用户设置的偏移位置，单位：us
     */
    int64_t userSeekUs;

    /**
     * 多媒体文件信息
     */
    Hello::MediaInfo mediaInfo;

    /**
     * 组件：数据加载（解协议 + 解复用）
     */
    std::shared_ptr<HelloDataReader> dataReader;

    /**
     * 组件：音频解码器
     */
    std::shared_ptr<HelloAVDecoder> audioDecoder;

    /**
     * 组件：音频解码器
     */
    std::shared_ptr<HelloAVDecoder> videoDecoder;

    /**
     * 组件：音频播放器
     */
    std::shared_ptr<HelloAudioPlayer> audioPlayer;

    /**
     * 组件：视频播放器
     */
    std::shared_ptr<HelloVideoPlayer> videoPlayer;

    /**
     * 缓存画面输出surface，视频播放器可能晚创建
     */
    void *pendingSurface;

    /**
     * 当前播放时间段
     */
    PlayRange currentPlayRange;

    /**
     * 已解码数据范围，以主时钟为准
     */
    PlayRange decodeBufferRange;

    /**
     * 设置的资源
     */
    std::string uri;

    /**
     * 播放器当前状态
     */
    std::atomic<PlayState> state;

    /**
     * 播放器线程，处理：控制响应，状态回调
     */
    NHandlerThread *playerThread;
    NHandler *playerHandler;

    /**
     * 音视频同步
     */
    std::shared_ptr<HelloAVSync> avSync;

    /**
     * Seek操作相关数据记录，一次性消费
     */
    std::shared_ptr<SeekAction> seekAction;
};


#endif //HELLOPLAYER_HELLOPLAYER_HPP
