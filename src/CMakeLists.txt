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


set(YAMLCPP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/window/yaml-cpp")
set(YAMLCPP_INCLUDE_DIR "${YAMLCPP_ROOT}/include")
set(YAMLCPP_LIB_RELEASE "${YAMLCPP_ROOT}/lib/libyaml-cpp.a")
set(YAMLCPP_LIB_DEBUG   "${YAMLCPP_ROOT}/lib/libyaml-cppd.a") # 按实际名字

add_library(yaml-cpp STATIC IMPORTED GLOBAL)
set_target_properties(yaml-cpp PROPERTIES
        IMPORTED_LOCATION_RELEASE "${YAMLCPP_LIB_RELEASE}"
        IMPORTED_LOCATION_DEBUG   "${YAMLCPP_LIB_DEBUG}"
        INTERFACE_INCLUDE_DIRECTORIES "${YAMLCPP_INCLUDE_DIR}"
)
target_compile_definitions(yaml-cpp INTERFACE YAML_CPP_STATIC_DEFINE)

#  如果你的 exe 也 include yaml-cpp：用 PUBLIC
target_link_libraries(RareVoyagerLib PUBLIC yaml-cpp)
