cmake_minimum_required(VERSION 2.8)

project(LuigiMaker)
add_subdirectory(./Cheets)

add_compile_definitions(COMPILING_CHEETS)

include_directories(src Cheets/src)
add_executable(LuigiMaker src/Program.cpp)

target_link_libraries(LuigiMaker PUBLIC Cheets)