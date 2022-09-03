cmake_minimum_required(VERSION 3.16)

project(Test VERSION 1.0)

include_directories(
	src
)

add_executable(Test
	src/Entity.cpp
	src/Registry.cpp
#	src/Scene.cpp
	src/Group.cpp
	src/main.cpp
)

set_property(TARGET Test PROPERTY CXX_STANDARD 20)