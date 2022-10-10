cmake_minimum_required(VERSION 3.16)

project(Test VERSION 1.0)

include_directories(
	src
)

add_executable(Test
	src/Main.cpp
 "src/UUID.h")

set_property(TARGET Test PROPERTY CXX_STANDARD 20)

install(
    TARGETS Test 
    RUNTIME DESTINATION "."
    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
)