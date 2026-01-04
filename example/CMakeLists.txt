include(../cmake/utils.cmake)


add_example_executable(logger_example logger/logger_example.cpp RareVoyagerLib)
add_example_executable(config_example config/config_example.cpp RareVoyagerLib)
#
add_executable(yaml_example config/yaml_example.cpp)
add_dependencies(yaml_example RareVoyagerLib)
target_link_libraries(yaml_example RareVoyagerLib)
