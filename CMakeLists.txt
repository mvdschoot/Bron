cmake_minimum_required(VERSION 3.0)

project(LuigiMaker VERSION 1.0)
add_subdirectory(Cheets)

add_compile_definitions(COMPILING_CHEETS)

include_directories(src Cheets/src Cheets/vendor)
add_executable(LuigiMaker src/Program.cpp)

target_link_libraries(LuigiMaker PUBLIC Cheets)