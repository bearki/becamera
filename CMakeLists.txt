# 指定Cmake工具所需的最小版本
cmake_minimum_required(VERSION 3.5)

# 配置项目名
project(BECAMERA)

# 是否编译为Release版本
set(CMAKE_BUILD_TYPE Release)

#告知当前使用的是交叉编译方式，必须配置
# 设置目标操作系统名称
# set(CMAKE_SYSTEM_NAME Linux)
# 设置目标操作系统架构
# set(CMAKE_SYSTEM_PROCESSOR arm64)
# 配置工具链所在目录
# set(TOOLCHAIN_DIR "C:\\msys64\\mingw64")
# 配置需要使用的C编译器
# set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc)
# 配置需要使用的C++编译器
# set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/arm-none-eabi-g++)

# 设定编译后生成的可执行二进制文件所在的目录
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)
# 设定编译后生成的库文件所在的目录（静态库或动态库都行）
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)

# 配置引用头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)
# 配置引用库链接目录（静态库或动态库都行）
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)

# 增加子目录
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/becamera)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
