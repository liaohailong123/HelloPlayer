import fileIo, { ReadTextOptions } from '@ohos.file.fs';

export class FileUtil {
  public static resourcesFile2SandboxFile(rawFilePath: string, sandBoxPath: string) {
    // 1、读取文件
    let uint8Array: Uint8Array = getContext().resourceManager.getRawFileContentSync(rawFilePath);
    // 2、创建沙箱文件
    let file: fileIo.File = fileIo.openSync(sandBoxPath, fileIo.OpenMode.READ_WRITE | fileIo.OpenMode.CREATE);
    // 3、resources下文件写入沙箱文件
    fileIo.writeSync(file.fd, uint8Array.buffer as ArrayBuffer);
    // 4、关闭文件
    fileIo.closeSync(file);
  }
}
