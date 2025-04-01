# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(helloplayer)

# 设置cpp编码标准 c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(3rd_part ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../3rdpart/ohos)

# ffmpeg 编译 x86版本报错：shared library text segment is not shareable
if (${OHOS_ARCH} EQUAL x86)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wall -v -Wl,--no-warn-shared-textrel")
endif ()


if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

# 设置ffmpeg目录位置
set(FFMPEG_PATH ${3rd_part}/ffmpeg)
# 添加ffmpeg头文件
include_directories(${FFMPEG_PATH}/${OHOS_ARCH}/include)

function(add_ffmpeg_library name)
    add_library(${name} STATIC IMPORTED)
    set_target_properties(
            ${name}
            PROPERTIES
            IMPORTED_LOCATION
            ${FFMPEG_PATH}/${OHOS_ARCH}/lib/lib${name}.a
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
include_directories(${FDK_AAC_PATH}/${OHOS_ARCH}/include) # 添加头文件
# 添加 fdk-aac 动态库和头文件依赖
add_library(fdk-aac STATIC IMPORTED)
set_target_properties(
        fdk-aac
        PROPERTIES
        IMPORTED_LOCATION
        ${FDK_AAC_PATH}/${OHOS_ARCH}/lib/libfdk-aac.a
)

# 设置mp3lame目录位置
set(MP3_LAME_PATH ${3rd_part}/libmp3lame-3.100)
include_directories(${MP3_LAME_PATH}/${OHOS_ARCH}/include) # 添加头文件
# 添加 fdk-aac 动态库和头文件依赖
add_library(mp3lame STATIC IMPORTED)
set_target_properties(
        mp3lame
        PROPERTIES
        IMPORTED_LOCATION
        ${MP3_LAME_PATH}/${OHOS_ARCH}/lib/libmp3lame.a
)

# 设置x264目录位置
set(X264_PATH ${3rd_part}/libx264)
include_directories(${X264_PATH}/${OHOS_ARCH}/include) # 添加头文件
# 添加 fdk-aac 动态库和头文件依赖
add_library(x264 STATIC IMPORTED)
set_target_properties(
        x264
        PROPERTIES
        IMPORTED_LOCATION
        ${X264_PATH}/${OHOS_ARCH}/lib/libx264.a
)

# 设置dav1d目录位置
set(DAV1D_PATH ${3rd_part}/libdav1d)
include_directories(${DAV1D_PATH}/${OHOS_ARCH}/include) # 添加头文件
# 添加 fdk-aac 动态库和头文件依赖
add_library(dav1d STATIC IMPORTED)
set_target_properties(
        dav1d
        PROPERTIES
        IMPORTED_LOCATION
        ${DAV1D_PATH}/${OHOS_ARCH}/lib/libdav1d.a
)

# 设置 openssl 目录位置
set(openssl ${3rd_part}/libopenssl)
include_directories(${openssl}/${OHOS_ARCH}/include) # 添加头文件
# 使用静态库
add_library(crypto STATIC IMPORTED)
set_target_properties(
        crypto
        PROPERTIES
        IMPORTED_LOCATION
        ${openssl}/${OHOS_ARCH}/lib/libcrypto.a
)

add_library(ssl STATIC IMPORTED)
set_target_properties(
        ssl
        PROPERTIES
        IMPORTED_LOCATION
        ${openssl}/${OHOS_ARCH}/lib/libssl.a
)

add_library(${CMAKE_PROJECT_NAME} SHARED napi_init.cpp)

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

# 播放器相关
add_subdirectory(player)


target_link_libraries(helloplayer PUBLIC
        nativehandler
        ${ffmpeg-libs}
        x264
        dav1d
        fdk-aac
        mp3lame
        ssl
        crypto
        # OpenGLES 3.0 + EGL
        EGL
        GLESv3
        # OHNativeWindow
        libnative_image.so
        libnative_buffer.so
        native_window
        # native_audiorender
        libohaudio.so
        # media decode
        libnative_media_codecbase.so
        libnative_media_core.so
        libnative_media_vdec.so
        m
        hilog_ndk.z
        libace_napi.z.so
        )