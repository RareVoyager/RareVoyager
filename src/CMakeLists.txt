file(GLOB_RECURSE APPLICATION_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE APPLICATION_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(RareVoyagerLib STATIC ${APPLICATION_SOURCES} ${APPLICATION_HEADERS})

target_include_directories(RareVoyagerLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(RareVoyagerLib PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

set(Boost_USE_STATIC_LIBS OFF CACHE BOOL "Use shared Boost libs" FORCE)

# 1. 修正路径变量：确保这里是 'window' (对应你提供的绝对路径)
set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/3rdparty/window/boost")
set(yaml-cpp_ROOT "${CMAKE_SOURCE_DIR}/3rdparty/window/yaml-cpp")

message(${BOOST_ROOT})
message(${yaml-cpp_ROOT})

# 帮助 find_package 找到 Config 文件
list(PREPEND CMAKE_PREFIX_PATH "${BOOST_ROOT}" "${yaml-cpp_ROOT}")

find_package(yaml-cpp REQUIRED CONFIG)
find_package(Boost REQUIRED CONFIG COMPONENTS context)

target_link_libraries(RareVoyagerLib PUBLIC
        yaml-cpp::yaml-cpp
        Boost::context
)

# 2. 修正包含路径：必须指向 'include' 目录
# 这样在代码中才能使用 #include <boost/context/fiber.hpp>
target_include_directories(RareVoyagerLib PUBLIC
        "${BOOST_ROOT}/include"
)