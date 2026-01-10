cmake_minimum_required(VERSION 3.31)
project(RareVoyager)

set(CMAKE_CXX_FLAGS "-Wno-macro-redefined")

include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (MSVC)
    add_compile_options(/utf-8)
endif()

add_subdirectory(src)
add_subdirectory(example)