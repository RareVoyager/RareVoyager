# 收集源文件中的cpp和h文件
#file(GLOB_RECURSE APPLICATION_SOURCES
#        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
#        "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cpp"
#)
#file(GLOB_RECURSE APPLICATION_HEADERS
#        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
#        "${CMAKE_CURRENT_SOURCE_DIR}/private/*.h"
#)

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
#set(FFMPEG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/windows/ffmpeg")
#set(FFMPEG_INCLUDE_DIR "${FFMPEG_ROOT}/include")
#set(FFMPEG_LIB_DIR "${FFMPEG_ROOT}/lib")
#
## 添加 FFmpeg 的 include 路径
#target_include_directories(CamLinker PRIVATE ${FFMPEG_INCLUDE_DIR})
#
## 手动指定需要的 FFmpeg 库
#target_link_libraries(CamLinker PRIVATE
#        ${FFMPEG_LIB_DIR}/avcodec.lib
#        ${FFMPEG_LIB_DIR}/avformat.lib
#        ${FFMPEG_LIB_DIR}/avutil.lib
#        ${FFMPEG_LIB_DIR}/swscale.lib
#        ${FFMPEG_LIB_DIR}/swresample.lib
#        ${FFMPEG_LIB_DIR}/avdevice.lib
#        ${FFMPEG_LIB_DIR}/avfilter.lib
#)