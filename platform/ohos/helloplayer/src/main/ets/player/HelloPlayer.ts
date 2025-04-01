import api from 'libhelloplayer.so';
import { HelloPlayerMediaInfo } from "./HelloPlayerMediaInfo"
import { HelloPlayerConfig } from "./HelloPlayerConfig"
import { HelloPlayerCallback } from "./HelloPlayerCallback"
import { HelloPlayerState, HelloPlayerStateValues } from './HelloPlayerState';
import { HelloPlayError, HelloPlayErrorValues } from './HelloPlayError';


/**
 * 播放器上层
 */
export class HelloPlayer {
  private readonly mPtr: number = 0
  // 回调接口
  private callback: HelloPlayerCallback | null = null
  // 缓存数据
  private mediaInfo: HelloPlayerMediaInfo | null = null
  private currentState: HelloPlayerState = HelloPlayerState.Idle
  private readonly surfaces: string[] = []

  public constructor() {
    this.mPtr = api.nativeInit(
      this,
      this.OnPreparedListener,
      this.OnPlayStateChangedListener,
      this.OnErrorInfoListener,
      this.OnCurrentPositionListener,
      this.OnBufferPositionListener,
    );
  }

  public setConfig(config: HelloPlayerConfig) {
    api.nativeSetConfig(this.mPtr, config);
  }

  public getConfig(): HelloPlayerConfig {
    return api.nativeGetConfig(this.mPtr);
  }

  public getMediaInfo(): HelloPlayerMediaInfo | null {
    return this.mediaInfo;
  }

  public getDurationUs(): number {
    return this.mediaInfo != null ? this.mediaInfo.getDurationUs() : 0;
  }

  public setDataSource(url: string): void {
    api.nativeSetDataSource(this.mPtr, url);
  }

  public addSurface(surfaceId: string): void {
    if (this.surfaces.includes(surfaceId)) {
      return;
    }
    this.surfaces.push(surfaceId);
    api.nativeAddSurface(this.mPtr, surfaceId);
  }

  public removeSurface(surfaceId: string): void {
    const index = this.surfaces.indexOf(surfaceId);
    if (index !== -1) {
      api.nativeRemoveSurface(this.mPtr, surfaceId);
      this.surfaces.splice(index, 1); // 从数组中删除元素
    }
  }

  public isPrepared(): boolean {
    return this.currentState > HelloPlayerState.Preparing;
  }

  public isPlaying(): boolean {
    return this.currentState == HelloPlayerState.Playing;
  }

  public prepare2(ptsUs: number): void {
    api.nativePrepare(this.mPtr, ptsUs);
  }

  public prepare(): void {
    api.nativePrepare(this.mPtr, 0);
  }

  public start(): void {
    api.nativeStart(this.mPtr);
  }

  public pause(): void {
    api.nativePause(this.mPtr);
  }

  public seekTo(ptsUs: number, autoPlay: boolean): void {
    api.nativeSeekTo(this.mPtr, ptsUs, autoPlay);
  }

  public setVolume(volume: number): void {
    api.nativeSetVolume(this.mPtr, volume);
  }

  public setSpeed(speed: number): void {
    api.nativeSetSpeed(this.mPtr, speed);
  }

  public reset(): void {
    this.surfaces.length = 0
    api.nativeReset(this.mPtr);
  }

  public release(): void {
    api.nativeRelease(this.mPtr);
  }


  // set listeners
  public setCallback(callback: HelloPlayerCallback) {
    this.callback = callback;
  }

  /**
   * Called by native
   *
   * @param info 多媒体信息
   */
  OnPreparedListener(info: HelloPlayerMediaInfo) {
    this.currentState = HelloPlayerState.Prepared;
    this.mediaInfo = info;
    this.callback?.onPrepared(info);
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
  OnPlayStateChangedListener(from: number, to: number) {
    // 获取枚举值
    let fromState: HelloPlayerState = HelloPlayerStateValues[from];
    let toState: HelloPlayerState = HelloPlayerStateValues[to];
    this.currentState = toState;

    this.callback?.onPlayStateChanged(fromState, toState);

    // 处理缓冲区 loading 逻辑
    if (toState === HelloPlayerState.Preparing || toState === HelloPlayerState.Seeking) {
      this.callback?.onBufferLoadStart();
    } else if (toState === HelloPlayerState.Prepared || toState === HelloPlayerState.SeekEnd) {
      this.callback?.onBufferLoadEnd();
    }
  }

  /**
   * Called by native
   *
   * @param error 错误码
   * @param msg   错误信息
   */
  OnErrorInfoListener(error: number, msg: string) {
    // 假设 callback 已定义并且 onErrorInfo 方法已实现
    if (this.callback) {
      let target: HelloPlayError = HelloPlayError.UnKnown;
      for (let errorCode of HelloPlayErrorValues) {
        if (errorCode === error) {
          target = errorCode;
          break;
        }
      }
      this.callback.onErrorInfo(target, msg);
    }
  }

  /**
   * Called by native
   *
   * @param ptsUs      回调当前播放时间，单位：us
   * @param durationUs 总时长，单位：us
   */
  OnCurrentPositionListener(ptsUs: number, durationUs: number) {
    this.callback?.onCurrentPosition(ptsUs, durationUs);
  }

  /**
   * Called by native
   *
   * @param startUs    缓冲区起始时间，单位：us
   * @param endUs      缓冲区结束时间，单位：us
   * @param durationUs 总时长，单位：us
   */
  OnBufferPositionListener(startUs: number, endUs: number, durationUs: number) {
    this.callback?.onBufferPosition(startUs, endUs, durationUs);
  }
}