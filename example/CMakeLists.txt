file(GLOB_RECURSE EXAMPLE_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/*.cpp
)

add_executable(RareVoyager
        ${EXAMPLE_FILE}
)

target_link_libraries(RareVoyager PRIVATE RareVoyagerLib)
