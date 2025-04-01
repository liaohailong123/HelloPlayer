export class HelloPlayerMediaInfo {
  public url: string;

  public startOffsetUs: number; // 播放起始偏移量
  public audioDurationUs: number; // 音频播放时长，单位：us
  public videoDurationUs: number; // 视频播放时长，单位：us

  public sampleRate: number; // 音频采样率
  public channelCount: number; // 音频声道数
  /**
   * samplefmt.h
   * enum AVSampleFormat {
   * AV_SAMPLE_FMT_NONE = -1,
   * AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
   * AV_SAMPLE_FMT_S16,         ///< signed 16 bits
   * AV_SAMPLE_FMT_S32,         ///< signed 32 bits
   * AV_SAMPLE_FMT_FLT,         ///< float
   * AV_SAMPLE_FMT_DBL,         ///< double
   * <p>
   * AV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
   * AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
   * AV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
   * AV_SAMPLE_FMT_FLTP,        ///< float, planar
   * AV_SAMPLE_FMT_DBLP,        ///< double, planar
   * AV_SAMPLE_FMT_S64,         ///< signed 64 bits
   * AV_SAMPLE_FMT_S64P,        ///< signed 64 bits, planar
   * <p>
   * AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
   * };
   */
  public sampleFmt: number; // 音频样本位深 AVSampleFormat

  public width: number; // 视频宽
  public height: number; // 视频高
  public frameRate: number; // 视频播放帧率

  /**
   * 主时钟类型：
   * AUDIO=0 VIDEO=1 参考：IAVMediaType.hpp
   */
  public masterClockType: number;

  /**
   * 是否为直播流
   */
  public isLiveStreaming: boolean;


  public getDurationUs(): number {
    return this.masterClockType == 0 ? this.audioDurationUs : this.videoDurationUs;
  }
}