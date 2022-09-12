# 指定Cmake工具所需的最小版本
cmake_minimum_required(VERSION 3.5)

# 配置源文件
SET(LIBBECAMERA_SRC becamera.cpp)

# 配置静态库生成参数
ADD_LIBRARY(becamera_static STATIC ${LIBBECAMERA_SRC})
# 指定静态库输出文件名
SET_TARGET_PROPERTIES(becamera_static PROPERTIES OUTPUT_NAME "becamera")
SET_TARGET_PROPERTIES(becamera_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# 配置动态库生成参数
ADD_LIBRARY(becamera_shared SHARED ${LIBBECAMERA_SRC})
# 指定动态库输出文件名
SET_TARGET_PROPERTIES(becamera_shared PROPERTIES OUTPUT_NAME "becamera")
SET_TARGET_PROPERTIES(becamera_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# 指定头文件存放目录
INSTALL(FILES becamera.h DESTINATION include)
# 指定静态库，动态库存放目录
# ARCHIVE-静态库，LIBRATY-动态库，RUNTIME-可执行二进制文件
INSTALL(TARGETS becamera_static ARCHIVE DESTINATION lib)
INSTALL(TARGETS becamera_shared LIBRARY DESTINATION lib)
# 指定二进制存放目录
# INSTALL(TARGETS becamera_shared LRUNTIME DESTINATION lib)
