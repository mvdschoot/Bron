cmake_minimum_required(VERSION 3.16)

project(LuigiMaker VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(Steve)

add_compile_definitions(COMPILING_CHEETS)
add_compile_definitions(CH_PROFILE)


add_executable(LuigiMaker
	src/Core/App.cpp
	src/Core/AppLayer.cpp
)
target_include_directories(LuigiMaker 
	PUBLIC .
	PUBLIC src
)

set_property(TARGET LuigiMaker PROPERTY CXX_STANDARD 20)
target_link_libraries(LuigiMaker PUBLIC Steve)
target_link_libraries(LuigiMaker PUBLIC VendorLibrary)
