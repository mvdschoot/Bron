cmake_minimum_required(VERSION 3.16)

project(LuigiMaker VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(Steve)

# add_definitions(CH_PROFILE)
get_target_property(SPDLOG_OPTIONS spdlog INTERFACE_LINK_OPTIONS)
message(STATUS "spdlog INTERFACE_LINK_OPTIONS: ${SPDLOG_OPTIONS}")

add_executable(LuigiMaker
	src/Core/App.cpp
	src/Core/AppLayer.cpp
	src/Core/EditorLayer.cpp
	src/Core/IconManagement.cpp
 
	src/Core/Panels/SceneHierarchy.cpp
	src/Core/Panels/ComponentRegistry.cpp

)

target_include_directories(LuigiMaker 
	PUBLIC .
	PUBLIC src
)

set_property(TARGET LuigiMaker PROPERTY CXX_STANDARD 20)
target_link_libraries(LuigiMaker PUBLIC Steve)
target_link_libraries(LuigiMaker PUBLIC VendorLibrary)

# add_definitions(-w)  # re-enable only if the noise gets unmanageable