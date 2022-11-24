# 指定Cmake工具所需的最小版本
cmake_minimum_required(VERSION 3.5)

# 配置相关源文件
set(LIBBECAMERA_SRC becamera.cpp convert.cpp)

# 配置静态库生成参数
add_library(becamera_static STATIC ${LIBBECAMERA_SRC})
# 指定静态库输出文件名
set_target_properties(becamera_static PROPERTIES OUTPUT_NAME "becamera")
set_target_properties(becamera_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# 配置动态库生成参数
add_library(becamera_shared SHARED ${LIBBECAMERA_SRC})
# 指定动态库输出文件名
set_target_properties(becamera_shared PROPERTIES OUTPUT_NAME "becamera")
set_target_properties(becamera_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# 指定make install后头文件存放目录
install(FILES becamera.h DESTINATION include)
# 指定make install后静态库，动态库存放目录
# ARCHIVE-静态库，LIBRATY-动态库，RUNTIME-可执行二进制文件
install(TARGETS becamera_static ARCHIVE DESTINATION lib)
install(TARGETS becamera_shared LIBRARY DESTINATION lib)
