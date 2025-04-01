export class HelloPlayerConfig {
  /**
   * 缓冲区大小，单位：us
   */
  public bufferDurationUs: number = 10 * 1000 * 1000; // 默认10s

  /**
   * 缓冲区扩充因子
   */
  public bufferLoadMoreFactor: number = 0.75;
  /**
   * 是否启用 硬件 解码视频
   */
  public useHardware: boolean = false;
  /**
   * 自动播放
   */
  public autoPlay: boolean = false;
  /**
   * 循环播放
   */
  public loopPlay: boolean = false;
  /**
   * 音量大小 0.0=静音 1.0=原始音量 大于1.0=扩大音量
   */
  public volume: number = 1.0;
  /**
   * 播放速率 [0.5,10.0]
   */
  public speed: number = 1.0;

  /**
   * IO连接(网络IO,文件IO)超时,默认15s
   */
  public ioTimeoutUs: number = 1000 * 1000 * 15;
}