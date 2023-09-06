add_executable(test_main test_case1.cpp)
target_include_directories(test_main PRIVATE ${CMAKE_SOURCE_DIR}/include)

#add_library(bplusUtils PRIVATE bplusUtils.cpp)
#target_include_directories(bplusUtils ${soucre_dir}/include)

#target_link_directories(main PRIVATE bplusUtils)

# 头文件路径
include_directories(
    /usr/local/include/gtest
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/gtest
)

# 生成可执行程序的文件,使用通配符指定
file(GLOB CPP_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c*
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/gtest_main.c*
)

# 依赖库列表
target_link_libraries(test_main 
    gtest
    gtest_main
    pthread
)


