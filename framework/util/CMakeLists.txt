# 找出当前目录下所有 hpp + cpp 文件
file(GLOB SRC_LIST "*.cpp" "*.hpp" "*.c" "*.h")

target_sources(${CMAKE_PROJECT_NAME}
        PRIVATE
        ${SRC_LIST}
        )


# h26x解析
add_subdirectory(h26x_sps_parse)