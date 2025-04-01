export const enum HelloPlayerState {
  Idle,
  Preparing,
  Prepared,
  Playing,
  Seeking,
  SeekEnd,
  Paused,
  Complete
}

// 手动创建映射数组
export const HelloPlayerStateValues: HelloPlayerState[] = [
  HelloPlayerState.Idle,
  HelloPlayerState.Preparing,
  HelloPlayerState.Prepared,
  HelloPlayerState.Playing,
  HelloPlayerState.Seeking,
  HelloPlayerState.SeekEnd,
  HelloPlayerState.Paused,
  HelloPlayerState.Complete
];
