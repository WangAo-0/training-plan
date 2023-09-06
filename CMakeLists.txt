cmake_minimum_required(VERSION 3.16.3)
project(vscode_practice)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(third/googletest)