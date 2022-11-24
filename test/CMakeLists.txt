# 指定Cmake工具所需的最小版本
cmake_minimum_required(VERSION 3.5)

# 添加可执行文件
add_executable(becamera_test becamera_test.cpp)

# 指定需要链接的库
target_link_libraries(becamera_test PRIVATE 
    becamera 
    avdevice
)
