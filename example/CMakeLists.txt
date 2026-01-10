include(../cmake/utils.cmake)
include(../cmake/sylar.cmake)

add_example_executable(logger_example logger/logger_example.cpp RareVoyagerLib)
force_redefine_file_macro_for_sources(logger_example)
add_example_executable(config_example config/config_example.cpp RareVoyagerLib)
add_example_executable(yaml_example config/yaml_example.cpp RareVoyagerLib)
add_example_executable(thread_example thread/thread_example.cpp RareVoyagerLib)
