package com.example.helloplayer.util;


import android.content.Context;
import android.content.res.AssetManager;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;


/**
 * create by liaohailong
 * 2024/10/13 13:29
 * desc:
 */
public class FileUtils {


    // 拷贝 assets 目录到 app 的内置存储
    public static void copyAssets(Context context, String assetDir, String destinationDir) {
        AssetManager assetManager = context.getAssets();
        String[] files = null;
        try {
            // 获取 assets 目录中的文件和子目录
            files = assetManager.list(assetDir);
            if (files != null) {
                // 创建目标目录
                File outDir = new File(context.getFilesDir(), destinationDir);
                if (!outDir.exists()) {
                    outDir.mkdirs();
                }

                for (String file : files) {
                    // 递归调用，检查是否为目录
                    if (assetManager.list(assetDir + "/" + file).length > 0) {
                        // 是目录，递归拷贝
                        copyAssets(context, assetDir + "/" + file, destinationDir + "/" + file);
                    } else {
                        // 不是目录，拷贝文件
                        copyAssetFileToInternalStorage(context, assetDir + "/" + file, destinationDir + "/" + file);
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // 拷贝单个文件
    private static void copyAssetFileToInternalStorage(Context context, String assetFileName, String destinationFileName) {
        AssetManager assetManager = context.getAssets();
        InputStream in = null;
        OutputStream out = null;
        try {
            // 打开 assets 文件
            in = assetManager.open(assetFileName);

            // 内置存储路径
            File outFile = new File(destinationFileName);
            if (!outFile.exists()) {
                File parentFile = outFile.getParentFile();
                if (!parentFile.exists()) {
                    parentFile.mkdirs();
                }
                outFile.createNewFile();
            }
            out = new FileOutputStream(outFile);

            // 缓冲区
            byte[] buffer = new byte[1024];
            int read;

            // 开始拷贝
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }
            out.flush();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (in != null) {
                    in.close();
                }
                if (out != null) {
                    out.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

}
