import { hilog } from '@kit.PerformanceAnalysisKit';
import { HelloPlayer } from './HelloPlayer';
import { HelloPlayerCallback } from './HelloPlayerCallback';
import { HelloPlayerConfig } from './HelloPlayerConfig';
import { HelloPlayerMediaInfo } from './HelloPlayerMediaInfo';
import { HelloPlayError } from './HelloPlayError';
import { HelloPlayerState } from './HelloPlayerState';

import { common } from '@kit.AbilityKit';

const DOMAIN = 0x0000;

@Component
export struct HelloPlayerView {
  viewController: HelloPlayerViewController | null = null
  player: HelloPlayer = new HelloPlayer()
  @State mediaInfo: HelloPlayerMediaInfo | null = null
  private config: HelloPlayerConfig = new HelloPlayerConfig()
  private callback: HelloPlayerCallback = {
    onPrepared: (mediaInfo: HelloPlayerMediaInfo): void => {
      this.mediaInfo = mediaInfo
      this.videoWidth = mediaInfo.width
      this.videoHeight = mediaInfo.height
      this.log(`mediaInfo.isLiveStreaming[${mediaInfo.isLiveStreaming}]`);
      if (this.surfaceId != null) {
        this.player.addSurface(this.surfaceId);
      }
    },
    onPlayStateChanged: (from: HelloPlayerState, to: HelloPlayerState): void => {
      this.btnText = to == HelloPlayerState.Playing ? "暂停" : "播放"
      this.log(`onPlayStateChanged from ${from} to ${to}`);
    },
    onErrorInfo: (errorCode: HelloPlayError, message: string): void => {
      this.log(`onErrorInfo errorCode: ${errorCode} message: ${message}`);
    },
    onBufferLoadStart: (): void => {
      this.log(`onBufferLoadStart`);
    },
    onBufferLoadEnd: (): void => {
      this.log(`onBufferLoadEnd`);
    },
    onCurrentPosition: (ptsUs: number, durationUs: number): void => {
      this.currentMs = ptsUs / 1000;
      this.durationMs = durationUs / 1000;
      // this.log(`onCurrentPosition pts: ${ptsUs / 1000}ms duration: ${durationUs / 1000}ms`);
    },
    onBufferPosition: (startUs: number, endUs: number, durationUs: number): void => {
      // this.log(`onBufferPosition startUs: ${startUs / 1000}ms endUs: ${endUs / 1000}ms duration: ${durationUs / 1000}ms`);
    }
  }
  @State videoWidth: number = 0.0
  @State videoHeight: number = 0.0
  @State surfaceId: string | null = null
  @State durationMs: number = 0
  @State currentMs: number = 0
  @State btnText: string = "播放"
  controller: XComponentController = new XComponentController();
  private context = getContext(this) as common.UIAbilityContext;
  filesDir = this.context.filesDir

  aboutToAppear(): void {
    this.viewController?.attach(this)
  }

  build() {
    Stack({ alignContent: Alignment.Bottom }) {

      XComponent({
        id: "HelloPlayerSurface",
        type: XComponentType.SURFACE,
        controller: this.controller
      }).onLoad(() => {
        this.surfaceId = this.controller.getXComponentSurfaceId();
        this.log(`onLoad surfaceId: ${this.surfaceId}`);
        if (this.player.isPrepared()) {
          this.player.addSurface(this.surfaceId);
        }
      }).aspectRatio(1.0 / 1.0)

      if (this.surfaceId != null) {
        Row() {
          Button(this.btnText, {
            type: ButtonType.Normal
          }).onClick(() => {

            this.log(`this.filesDir: ${this.filesDir}`)
            if (this.player.isPlaying()) {
              this.player.pause()
            } else {
              this.player.start()
            }
          }).margin({ right: 5 })

          Slider({
            min: 0.0,
            max: 1.0,
            value: this.getCurrentProgress(),
            step: 0.01
          })
            .layoutWeight(1)
            .onChange((value: number, mode: SliderChangeMode) => {
              if (SliderChangeMode.End == mode) {
                this.log(`slider onChange(${value})`)
                let autoPlay = this.player.isPlaying()
                let ptsUs = this.durationMs * 1000 * value;
                this.player.seekTo(ptsUs, autoPlay)
              }
            })
            .trackColor(0x80FFFFFF).margin({ right: 5 })

          if (this.mediaInfo != null) {
            if (this.mediaInfo.isLiveStreaming) {
              Text("").fontColor(0xFFFFFF)
              Text("").fontColor(0xFFFFFF)
            } else {
              Text(this.formatTime(this.currentMs)).fontColor(0xFFFFFF)
              Text("/").fontColor(0xFFFFFF)
            }
          }
          Text(this.formatTime(this.durationMs)).fontColor(0xFFFFFF)
        }
        .padding({ left: 5, right: 5 })

      }
    }
    .width("100%")
  }

  public setDataSource(url: string, useHardware: boolean) {
    // 默认配置
    this.config.bufferDurationUs = 10 * 1000 * 1000; // 合理设置，不用太大
    this.config.autoPlay = true; // 自动起播
    this.config.loopPlay = true; // 循环播放
    this.config.speed = 1.0; // 播放速率，不宜设置过高，视频解码器跟不上
    this.config.volume = 1.0; // 音量大小，不宜设置过高 0到10的范围效果明显
    this.config.useHardware = useHardware;

    if (this.player.isPrepared()) {
      this.player.reset();
    }
    this.player.setConfig(this.config);
    this.player.setDataSource(url);
    this.player.setCallback(this.callback);
    this.player.prepare();
  }

  private formatTime(milliseconds: number | null): string {
    if (typeof milliseconds !== "number" || milliseconds < 0) {
      return "00:00";
    }

    let totalSeconds = Math.floor(milliseconds / 1000);
    let hours = Math.floor(totalSeconds / 3600);
    let minutes = Math.floor((totalSeconds % 3600) / 60);
    let secs = Math.floor(totalSeconds % 60);

    let formattedTime = hours > 0
      ? [hours, minutes, secs].map(n => String(n).padStart(2, '0')).join(':')
      : [minutes, secs].map(n => String(n).padStart(2, '0')).join(':');

    return formattedTime;
  }

  private getCurrentProgress(): number {
    if (this.durationMs == 0) {
      return 0.0
    }
    return this.currentMs / this.durationMs;
  }

  private log(message: string): void {
    hilog.info(DOMAIN, 'HelloPlayerView', message);
  }
}

export class HelloPlayerViewController {
  view: HelloPlayerView | null = null

  attach(view: HelloPlayerView) {
    this.view = view
  }

  public play(url: string, useHardware: boolean) {
    this.view?.setDataSource(url, useHardware)
  }

  setSpeed(speed: number) {
    this.view?.player.setSpeed(speed)
  }

  setVolume(volume: number) {
    this.view?.player.setVolume(volume)
  }
}