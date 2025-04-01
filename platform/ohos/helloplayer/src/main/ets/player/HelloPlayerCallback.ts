import { HelloPlayerMediaInfo } from "helloplayer/ets/player/HelloPlayerMediaInfo"
import { HelloPlayerState } from "helloplayer/ets/player/HelloPlayerState"
import { HelloPlayError } from "helloplayer/ets/player/HelloPlayError"

export interface HelloPlayerCallback {

  /**
   * @param mediaInfo 播放器准备好后回调，多媒体信息
   */
  onPrepared: (mediaInfo: HelloPlayerMediaInfo) => void;

  /**
   * @param from 播放状态(上次)回调
   * @param to 播放状态(本次)回调
   */
  onPlayStateChanged: (from: HelloPlayerState, to: HelloPlayerState) => void;

  /**
   * @param errorCode 错误码
   * @param message 错误信息
   */
  onErrorInfo: (errorCode: HelloPlayError, message: string) => void;

  /**
   * 数据加载开始
   */
  onBufferLoadStart: () => void;

  /**
   * 数据加载结束
   */
  onBufferLoadEnd: () => void;

  /**
   * @param ptsUs      当前主时钟播放时间，单位：us
   * @param durationUs 总时长，单位：us
   */
  onCurrentPosition: (ptsUs: number, durationUs: number) => void;

  /**
   * 当前缓冲区时间
   *
   * @param startUs 缓冲区起始时间，单位：us
   * @param endUs   缓冲区结束时间，单位：us
   * @param durationUs 总时长，单位：us
   */
  onBufferPosition: (startUs: number, endUs: number, durationUs: number) => void;
}