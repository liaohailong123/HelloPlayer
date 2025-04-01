# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("helloplayer")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.

# 设置cpp编码标准 c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(3rd_part ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../3rdpart/android)

# 设置ffmpeg目录位置
set(FFMPEG_PATH ${3rd_part}/ffmpeg)
# 添加ffmpeg头文件
include_directories(${FFMPEG_PATH}/${ANDROID_ABI}/include)

function(add_ffmpeg_library name)
    add_library(${name} STATIC IMPORTED)
    set_target_properties(
            ${name}
            PROPERTIES
            IMPORTED_LOCATION
            ${FFMPEG_PATH}/${ANDROID_ABI}/lib/lib${name}.a
    )
endfunction()

# 依赖ffmpeg-libavformat
add_ffmpeg_library(avformat)
# 依赖ffmpeg-libavcodec
add_ffmpeg_library(avcodec)
# 依赖ffmpeg-libavfilter
add_ffmpeg_library(avfilter)
# 依赖ffmpeg-libswresample
add_ffmpeg_library(swresample)
# 依赖ffmpeg-libswscale
add_ffmpeg_library(swscale)
# 依赖ffmpeg-libavutil
add_ffmpeg_library(avutil)
# 所有ffmpeg依赖整合到一起，ffmpeg依赖zlib，需要添加z库
# 特别注意：依赖静态库的时候需要注意顺序 参考：https://www.jianshu.com/p/6fc1a2adac4f
set(ffmpeg-libs
        avfilter
        avformat
        avcodec
        avutil
        swresample
        swscale
        z
)

# 设置fdk-aac目录位置
set(FDK_AAC_PATH ${3rd_part}/libfdk-aac-2.0.3)
include_directories(${FDK_AAC_PATH}/${ANDROID_ABI}/include) # 添加头文件
# 添加 fdk-aac 动态库和头文件依赖
add_library(fdk-aac STATIC IMPORTED)
set_target_properties(
        fdk-aac
        PROPERTIES
        IMPORTED_LOCATION
        ${FDK_AAC_PATH}/${ANDROID_ABI}/lib/libfdk-aac.a
)

# 设置mp3lame目录位置
set(MP3_LAME_PATH ${3rd_part}/libmp3lame-3.100)
include_directories(${MP3_LAME_PATH}/${ANDROID_ABI}/include) # 添加头文件
# 添加 fdk-aac 动态库和头文件依赖
add_library(mp3lame STATIC IMPORTED)
set_target_properties(
        mp3lame
        PROPERTIES
        IMPORTED_LOCATION
        ${MP3_LAME_PATH}/${ANDROID_ABI}/lib/libmp3lame.a
)

# 设置x264目录位置
set(X264_PATH ${3rd_part}/libx264)
include_directories(${X264_PATH}/${ANDROID_ABI}/include) # 添加头文件
# 添加 fdk-aac 动态库和头文件依赖
add_library(x264 STATIC IMPORTED)
set_target_properties(
        x264
        PROPERTIES
        IMPORTED_LOCATION
        ${X264_PATH}/${ANDROID_ABI}/lib/libx264.a
)

# 设置dav1d目录位置
set(DAV1D_PATH ${3rd_part}/libdav1d)
include_directories(${DAV1D_PATH}/${ANDROID_ABI}/include) # 添加头文件
# 添加 fdk-aac 动态库和头文件依赖
add_library(dav1d STATIC IMPORTED)
set_target_properties(
        dav1d
        PROPERTIES
        IMPORTED_LOCATION
        ${DAV1D_PATH}/${ANDROID_ABI}/lib/libdav1d.a
)

# 设置 openssl 目录位置
set(openssl ${3rd_part}/libopenssl)
include_directories(${openssl}/${ANDROID_ABI}/include) # 添加头文件
# 使用静态库
add_library(crypto STATIC IMPORTED)
set_target_properties(
        crypto
        PROPERTIES
        IMPORTED_LOCATION
        ${openssl}/${ANDROID_ABI}/lib/libcrypto.a
)

add_library(ssl STATIC IMPORTED)
set_target_properties(
        ssl
        PROPERTIES
        IMPORTED_LOCATION
        ${openssl}/${ANDROID_ABI}/lib/libssl.a
)

# 设置speex目录位置
set(speex ${3rd_part}/speex/speex-1.2.1)
include_directories(${speex}/${ANDROID_ABI}/include) # 添加头文件
# 添加动态库和头文件依赖
add_library(speex STATIC IMPORTED)
set_target_properties(
        speex
        PROPERTIES
        IMPORTED_LOCATION
        ${speex}/${ANDROID_ABI}/lib/libspeex.a
)

# 设置speexdsp目录位置
set(speexdsp ${3rd_part}/speex/speexdsp-1.2.1)
include_directories(${speexdsp}/${ANDROID_ABI}/include) # 添加头文件
# 添加动态库和头文件依赖
add_library(speexdsp STATIC IMPORTED)
set_target_properties(
        speexdsp
        PROPERTIES
        IMPORTED_LOCATION
        ${speexdsp}/${ANDROID_ABI}/lib/libspeexdsp.a
)

# Obo库
set(OBOE_DIR ${3rd_part}/oboe)
add_subdirectory(${OBOE_DIR} ./oboe-bin)
include_directories(${OBOE_DIR}/include)



add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
)

# - - - - - - - - - - - - - - - - Framework模块 - - - - - - - - - - - - - - - -

set(FRAMEWORK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../framework)

message("FRAMEWORK_PATH:")
message(${FRAMEWORK_PATH})

# 添加头文件 简化 #include "framework/xxx/xxx.hpp"
include_directories(${FRAMEWORK_PATH}/../)

# 工具库部分
add_subdirectory(${FRAMEWORK_PATH}/util ./util-bin)

# log日志部分
add_subdirectory(${FRAMEWORK_PATH}/log ./log-bin)

# NativeHandler部分
add_subdirectory(${FRAMEWORK_PATH}/handler ./handler-bin)

# player代码部分
add_subdirectory(${FRAMEWORK_PATH}/player ./player-bin)

# render代码部分 OpenGLES逻辑多端复用
add_subdirectory(${FRAMEWORK_PATH}/render ./render-bin)

# - - - - - - - - - - - - - - - - 平台侧实现 - - - - - - - - - - - - - - - -
# render部分
# EGL + OpenGL ES
add_definitions(-DEGL_EGLEXT_PROTOTYPES=1) # 打开 eglext.h 中的部分函数
add_subdirectory(render)

# JNI桥代码部分
add_subdirectory(jni)

# 播放器相关
add_subdirectory(player)


# 链接库
target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        nativehandler
        oboe
        EGL
        GLESv3
        ${ffmpeg-libs}
        x264
        dav1d
        fdk-aac
        mp3lame
        ssl
        crypto
        speex
        speexdsp
        android
        log
        m
        z
)