import { HelloPlayer } from "helloplayer/ets/player/HelloPlayer"
import { HelloPlayerConfig } from "helloplayer/ets/player/HelloPlayerConfig"
import { HelloPlayerMediaInfo } from "helloplayer/ets/player/HelloPlayerMediaInfo"


export const nativeInit: (
  this_arg: HelloPlayer,
  OnPreparedListener: (info: HelloPlayerMediaInfo) => void,
  OnPlayStateChangedListener: (from: number, to: number) => void,
  OnErrorInfoListener: (code: number, msg: string) => void,
  OnCurrentPositionListener: (ptsUs: number, durationUs: number) => void,
  OnBufferPositionListener: (startUs: number, endUs: number, durationUs: number) => void
) => number;

export const nativeSetConfig: (ptr: number, config: HelloPlayerConfig) => void;

export const nativeGetConfig: (ptr: number) => HelloPlayerConfig;

export const nativeSetDataSource: (ptr: number, url: string) => void;

export const nativeAddSurface: (ptr: number, surfaceId: string) => void;

export const nativeRemoveSurface: (ptr: number, surfaceId: string) => void;

export const nativePrepare: (ptr: number, ptsUs: number) => void;

export const nativeStart: (ptr: number) => void;

export const nativePause: (ptr: number) => void;

export const nativeSeekTo: (ptr: number, ptsUs: number, autoPlay: boolean) => void;

export const nativeSetVolume: (ptr: number, volume: number) => void;

export const nativeSetSpeed: (ptr: number, speed: number) => void;

export const nativeReset: (ptr: number) => void;

export const nativeRelease: (ptr: number) => void;