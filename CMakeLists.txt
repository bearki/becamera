# 指定Cmake工具所需的最小版本
cmake_minimum_required(VERSION 3.5)

# 配置项目名
PROJECT(BECAMERA)

#告知当前使用的是交叉编译方式，必须配置
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR arm64)

# 配置工具链所在目录
SET(TOOLCHAIN_DIR "C:\\msys64\\mingw64")
# 配置需要使用的C++编译器
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/arm-none-eabi-g++)
# 配置需要使用的C编译器
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc)

# 什么鸡巴玩意
# SET(GNU_FLAGS "-mfpu=vfp -fPIC")
# SET(CMAKE_CXX_FLAGS "${GNU_FLAGS}")
# SET(CMAKE_C_FLAGS "${GNU_FLAGS}")

# 配置交叉编译环境安装目录（相当于工作目录，一般会把标准库目录一起放入，可以存在多个目录）
SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_DIR} ${TOOLCHAIN_DIR}/arm-none-eabi/include ${TOOLCHAIN_DIR}/arm-none-eabi/lib)

# 配置引用头文件查找目录
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
# 配置引用库链接目录（静态库或动态库都行）
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib)

# 增加子目录
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/becamera)
 
#不一定需要设置
#从来不在指定目录下查找工具程序
# SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#只在指定目录下查找库文件
# SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#只在指定目录下查找头文件
# SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)