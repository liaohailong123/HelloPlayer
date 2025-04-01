# 找出当前目录下所有 hpp + cpp 文件
file(GLOB SRC_LIST "*.cpp" "*.hpp")

target_sources(${CMAKE_PROJECT_NAME}
        PRIVATE
        ${SRC_LIST}
        )


# 添加 entity 部分
add_subdirectory(entity)

# 添加 重要组件 部分
add_subdirectory(component)

# 添加 同步时钟 部分
add_subdirectory(clock)

# 硬件相关功能：音视频解码+渲染
add_subdirectory(device)