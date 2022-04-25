cmake_minimum_required(VERSION 2.8)

project(LuigiMaker)
add_subdirectory(./Cheets)

include_directories(src .)
add_executable(LuigiMaker Application.cpp)

target_link_libraries(LuigiMaker PUBLIC Cheets)