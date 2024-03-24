cmake_minimum_required(VERSION 3.16)

project(CheetsProject)

add_subdirectory("vendor")
add_subdirectory("src")
target_link_libraries(Steve
	PUBLIC VendorLibrary
)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
