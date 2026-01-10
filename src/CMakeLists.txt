file(GLOB_RECURSE APPLICATION_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(GLOB_RECURSE APPLICATION_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)


if(DEBUG)
message(${APPLICATION_SOURCES})
message(${APPLICATION_HEADERS})
endif ()
# 打成静态库
add_library(RareVoyagerLib STATIC
        ${APPLICATION_SOURCES}
        ${APPLICATION_HEADERS}
)

# 添加头文件搜索路径
target_include_directories(RareVoyagerLib PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_include_directories(RareVoyagerLib PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}"
)

## 未来指定三方库
# 步骤: 1.set 三方库的相对路径
#       2.添加include 路径
#       3.指定对应的lib库
set(Boost_NO_BOOST_CMAKE ON)
# 设置 Boost 根目录，FindBoost 会根据这个自动推导 include 和 lib 路径
set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/window/boost")
set(BOOST_INCLUDE_DIR "${BOOST_ROOT}/include/boost-1_90")

set(Boost_USE_STATIC_LIBS ON)

target_include_directories(RareVoyagerLib PUBLIC
        "${BOOST_INCLUDE_DIR}"
)


# 1. 告诉 CMake 去哪里找 yaml-cpp 的配置文件
# 指向 yaml-cpp 的根目录即可，CMake 会自动搜索其下的 lib/cmake 等子目录
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/window")

# 2. 查找包 (CONFIG 模式)
# 这会自动处理 include 路径、库路径、Debug/Release 切换，甚至会自动加上 YAML_CPP_STATIC_DEFINE
find_package(yaml-cpp REQUIRED CONFIG)

# 3. 链接
# yaml-cpp::yaml-cpp 是它官方提供的现代 Target 名称
target_link_libraries(RareVoyagerLib PUBLIC yaml-cpp::yaml-cpp)