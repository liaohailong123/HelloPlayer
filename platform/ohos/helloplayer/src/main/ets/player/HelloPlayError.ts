// 定义 HelloPlayError 枚举
export const enum HelloPlayError {
  UnKnown = -1,
  None = 0,
  NoDataSource = -100,
  NoStreamInDataSource = -101,
  NoAudioVideoStream = -102,
  NetworkTimeout = -400
}

// 手动创建枚举值的数组
export const HelloPlayErrorValues: HelloPlayError[] = [
  HelloPlayError.UnKnown,
  HelloPlayError.None,
  HelloPlayError.NoDataSource,
  HelloPlayError.NoStreamInDataSource,
  HelloPlayError.NoAudioVideoStream,
  HelloPlayError.NetworkTimeout
];