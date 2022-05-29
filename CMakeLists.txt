cmake_minimum_required(VERSION 3.16)

project(LuigiMaker VERSION 1.0)

add_subdirectory(Cheets)

add_compile_definitions(COMPILING_CHEETS)
add_compile_definitions(CH_PROFILE)

include_directories(
	src 
	Cheets/src 
	vendor/glad/include
	vendor/imgui
	vendor/imgui/backends
)
add_executable(LuigiMaker src/Program.cpp)

target_link_libraries(LuigiMaker PUBLIC Cheets)