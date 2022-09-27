# 指定Cmake工具所需的最小版本
cmake_minimum_required(VERSION 3.5)

# 配置项目名
PROJECT(BECAMERA)

#告知当前使用的是交叉编译方式，必须配置
# SET(CMAKE_SYSTEM_NAME Linux)
# SET(CMAKE_SYSTEM_PROCESSOR arm64)

# 配置工具链所在目录
# SET(TOOLCHAIN_DIR "C:\\msys64\\mingw64")
# 配置需要使用的C++编译器
# set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/arm-none-eabi-g++)
# 配置需要使用的C编译器
# set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc)

# 配置交叉编译环境安装目录（相当于工作目录，一般会把标准库目录一起放入，可以存在多个目录）
SET(CMAKE_FIND_ROOT_PATH /usr/include /usr/lib)

# 设定编译后生成的可执行二进制文件所在的目录
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)
# 设定编译后生成的库文件所在的目录
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)

# 配置引用头文件查找目录
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)
# 配置引用库链接目录（静态库或动态库都行）
LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# 增加子目录
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/becamera)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/test)
