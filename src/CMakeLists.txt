add_executable(main Main.cpp ../include/BPlusTree.pb.cc)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

target_link_libraries(main protobuf pthread)
#target_link_directories(main PRIVATE bplusUtils)