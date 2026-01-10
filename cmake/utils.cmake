# 简化测试代码里面的生成可执行文件的逻辑
# 参数名称解释
## test_config 生成的可执行文件的名称
## source_file 测试源文件
## link_lib 需要链接的库
function(add_example_executable test_config source_file link_lib)
    add_executable(${test_config} ${source_file})
    add_dependencies(${test_config} ${link_lib})
    target_link_libraries(${test_config} PRIVATE  ${link_lib})
    force_redefine_file_macro_for_sources(${test_config})
endfunction()