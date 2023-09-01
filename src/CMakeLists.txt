add_executable(main Main.cpp)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)

#add_library(bplusUtils PRIVATE bplusUtils.cpp)
#target_include_directories(bplusUtils ${soucre_dir}/include)

#target_link_directories(main PRIVATE bplusUtils)