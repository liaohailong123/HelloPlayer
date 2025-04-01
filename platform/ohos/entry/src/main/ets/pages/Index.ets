import { hilog } from '@kit.PerformanceAnalysisKit';
import { HelloPlayerView } from 'helloplayer/src/main/ets/player/HelloPlayerView';
import { HelloPlayerViewController } from 'helloplayer/src/main/ets/player/HelloPlayerView';

import { FileUtil } from '../utils/FileUtil';
import { common } from '@kit.AbilityKit';
import { List } from '@kit.ArkTS';

const DOMAIN = 0x0000;

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  @State volumePercent: number = 0.5
  private controller: HelloPlayerViewController = new HelloPlayerViewController()
  // app沙盒目录
  private context = getContext(this) as common.UIAbilityContext
  filesDir = this.context.filesDir

  onPageShow(): void {
    FileUtil.resourcesFile2SandboxFile("Sync-One2_Test_1080p_23.98_H.264_AAC_5.1.mp4",
      this.filesDir + "/Sync-One2_Test_1080p_23.98_H.264_AAC_5.1.mp4"
    )
    FileUtil.resourcesFile2SandboxFile("output_av1.mp4",
      this.filesDir + "/output_av1.mp4"
    )
  }

  build() {
    Column({
      space: 5
    }) {
      Text("HelloPlayer").fontSize(24).fontWeight(FontWeight.Bold).margin(16).align(Alignment.TopStart)

      // first line
      Row({
        space: 5
      }) {
        Button("软解H264(本地)").onClick(() => {
          let url = this.filesDir + "/Sync-One2_Test_1080p_23.98_H.264_AAC_5.1.mp4"
          this.controller.play(url, false)
        }).layoutWeight(1)
        Button("软解AV1(本地)").onClick(() => {
          let url = this.filesDir + "/output_av1.mp4"
          this.controller.play(url, false)
        }).layoutWeight(1)
        Button("软解H265(网络)").onClick(() => {
          let url =
            "http://demo-videos.qnsdk.com/bbk-H265-50fps.mp4"
          this.controller.play(url, false)
        }).layoutWeight(1)

      }.width("100%")

      // second line
      Row({
        space: 5
      }) {
        Button("硬解H264(本地)").onClick(() => {
          let url = "http://demo-videos.qnsdk.com/VR-Panorama-Equirect-Angular-4500k.mp4"
          this.controller.play(url, true)
        }).layoutWeight(1)
        Button("软解AV1(本地2)").onClick(() => {
          let url = this.filesDir + "/output_av1.mp4"
          this.controller.play(url, true)
        }).layoutWeight(1)

        Button("硬解H265(网络)").onClick(() => {
          let url =
            "http://demo-videos.qnsdk.com/bbk-H265-50fps.mp4"
          this.controller.play(url, true)
        }).layoutWeight(1)
      }.width("100%")

      // third line
      Row({
        space: 5
      }) {
        Button("硬解H265(直播)").onClick(() => {
          let url =
            "rtmp://liteavapp.qcloud.com/live/liteavdemoplayerstreamid"
          this.controller.play(url, true)
        }).layoutWeight(1)
        Button("").onClick(() => {

        }).layoutWeight(1).backgroundColor(0xFFFFFF)

        Button("").onClick(() => {

        }).layoutWeight(1).backgroundColor(0xFFFFFF)
      }.width("100%")

      Row({
        space: 5
      }) {
        Button("速率0.5X").onClick(() => {
          this.controller.setSpeed(0.5)
        }).layoutWeight(1)
        Button("速率1.0X").onClick(() => {
          this.controller.setSpeed(1.0)
        }).layoutWeight(1)
        Button("速率1.5X").onClick(() => {
          this.controller.setSpeed(1.5)
        }).layoutWeight(1)
        Button("速率2.0X").onClick(() => {
          this.controller.setSpeed(2.0)
        }).layoutWeight(1)
      }.width("100%")

      // 音频大小
      Row() {
        Text("音量:").fontColor(0x333333)

        Slider({
          min: 0.0,
          max: 1.0,
          value: this.volumePercent,
          step: 0.01
        })
          .layoutWeight(1)
          .onChange((value: number, mode: SliderChangeMode) => {

            this.volumePercent = value
            let volume = value * 2
            this.log(`slider onChange(${value}) volume(${volume})`)
            this.controller.setVolume(volume)

          }).margin({ right: 10 })

        Text(`${Math.round(this.volumePercent * 100)}`).fontColor(0x333333)
      }

      HelloPlayerView({
        viewController: this.controller
      }).width('100%')

    }.height("100%")
    .margin({ left: 10, right: 10 })
  }

  private log(message: string): void {
    hilog.info(DOMAIN, 'HelloPlayerView', message);
  }
}