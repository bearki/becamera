# 指定Cmake工具所需的最小版本
cmake_minimum_required(VERSION 3.5)

# 配置源文件
ADD_EXECUTABLE(main becamera_test.cpp)

# 指定库链接地址
TARGET_LINK_LIBRARIES(main PRIVATE 
    becamera 
    avdevice
    # avcodec
    # avformat
    # avutil
)
