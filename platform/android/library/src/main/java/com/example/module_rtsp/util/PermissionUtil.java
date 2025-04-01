package com.example.module_rtsp.util;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;

import java.util.ArrayList;
import java.util.List;

/**
 * create by liaohailong
 * time 2022/9/26 18:51
 * desc: 权限相关工具类
 */
public class PermissionUtil {

    /**
     * 定位权限
     */
    public static final String[] LOCATION_PERMISSIONS = new String[]{
            Manifest.permission.ACCESS_FINE_LOCATION,
            Manifest.permission.ACCESS_COARSE_LOCATION};

    /**
     * Api29及以上，后台服务获取位置需要申请，
     * api30之后，必须与LOCATION_PERMISSIONS分开获取，先拿到定位权限之后，再申请后台定位
     */
    public static final String[] LOCATION_BACKGROUND_PERMISSIONS = new String[]{
            Manifest.permission.ACCESS_BACKGROUND_LOCATION
    };

    /**
     * 存储权限
     */
    public static String[] STORAGE_PERMISSIONS = new String[]{
            Manifest.permission.READ_EXTERNAL_STORAGE};

    static {
        if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.Q) {
            STORAGE_PERMISSIONS = new String[]{
                    Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE};
        }
    }

    /**
     * 存储和相机权限
     */
    public static String[] STORAGE_CAMERA_PERMISSIONS = new String[]{
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.CAMERA
    };

    static {
        if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.Q) {
            STORAGE_CAMERA_PERMISSIONS = new String[]{
                    Manifest.permission.READ_EXTERNAL_STORAGE
                    , Manifest.permission.WRITE_EXTERNAL_STORAGE
                    , Manifest.permission.CAMERA};
        }
    }

    /**
     * 存储和麦克风权限
     */
    public static String[] STORAGE_RECORD_PERMISSIONS = new String[]{
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.RECORD_AUDIO};

    static {
        if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.Q) {
            STORAGE_RECORD_PERMISSIONS = new String[]{
                    Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.RECORD_AUDIO};
        }
    }

    /**
     * 存储&麦克风&相机权限
     */
    public static String[] STORAGE_RECORD_CAMERA_PERMISSIONS = new String[]{
            Manifest.permission.READ_EXTERNAL_STORAGE
            , Manifest.permission.RECORD_AUDIO
            , Manifest.permission.CAMERA};

    static {
        if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.Q) {
            STORAGE_RECORD_CAMERA_PERMISSIONS = new String[]{
                    Manifest.permission.READ_EXTERNAL_STORAGE
                    , Manifest.permission.WRITE_EXTERNAL_STORAGE
                    , Manifest.permission.RECORD_AUDIO
                    , Manifest.permission.CAMERA};
        }
    }

    /**
     * 检测是否有某组权限
     *
     * @param context     上下文对象
     * @param permissions 多个权限数组
     */
    public static boolean hasPermission(Context context, String[] permissions) {
        if (null == context || null == permissions || permissions.length <= 0) return false;

        for (String permission : permissions) {
            if (ActivityCompat.checkSelfPermission(context, permission) != PackageManager.PERMISSION_GRANTED) {
                //只有一个权限没有被赋予，则直接返回false
                return false;
            }
        }

        return true;
    }


    /**
     * 尝试去请求权限
     *
     * @param activity    当前界面，该参数必须要是Activity
     * @param permission  权限
     * @param requestCode 请求码，在{@link Activity#onRequestPermissionsResult(int, String[], int[])}监听回调
     * @return true表示需要去请求权限了，false表示该权限都已经获取到位，无需再请求
     */
    public static boolean requestIfNeed(@NonNull Activity activity,
                                        @NonNull String permission, int requestCode) {
        String[] permissions = {permission};
        return requestIfNeed(activity, permissions, requestCode);
    }

    /**
     * 尝试去请求权限
     *
     * @param activity    当前界面，该参数必须要是Activity
     * @param permissions 权限组
     * @param requestCode 请求码，在{@link Activity#onRequestPermissionsResult(int, String[], int[])}监听回调
     * @return true表示需要去请求权限了，false表示该权限都已经获取到位，无需再请求
     */
    public static boolean requestIfNeed(@NonNull Activity activity,
                                        @NonNull String[] permissions, int requestCode) {
        List<String> requestList = new ArrayList<>();

        for (String permission : permissions) {
            if (Build.VERSION.SDK_INT > Build.VERSION_CODES.Q) {
                // android 11 起，这个权限废弃了
                if (Manifest.permission.WRITE_EXTERNAL_STORAGE.equals(permission)) {
                    continue;
                }
            }
            if (PackageManager.PERMISSION_DENIED == ActivityCompat.checkSelfPermission(activity, permission)) {
                requestList.add(permission);
            }
        }

        if (requestList.isEmpty()) {
            return false;
        }

        ActivityCompat.requestPermissions(activity, requestList.toArray(new String[]{}), requestCode);
        return true;
    }


    /**
     * @param grantResults 权限请求结果
     * @return true表示所有权限都请求到了
     */
    public static boolean isGranted(int[] grantResults) {
        for (int grantResult : grantResults) {
            if (grantResult != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

}
