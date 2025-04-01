# 找出当前目录下所有 hpp + cpp 文件
file(GLOB SRC_LIST "*.cpp" "*.hpp")

target_sources(${CMAKE_PROJECT_NAME}
        PRIVATE
        ${SRC_LIST}
        )


# 添加 device 模块，提供平台侧 视频硬解码
add_subdirectory(device)