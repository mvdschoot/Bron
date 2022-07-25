cmake_minimum_required(VERSION 3.16)

project(Test VERSION 1.0)

include_directories(
	.
)

add_executable(Test
	Group.cpp
	main.cpp
)

set_property(TARGET Test PROPERTY CXX_STANDARD 20)