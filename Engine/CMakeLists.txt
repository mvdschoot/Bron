cmake_minimum_required(VERSION 3.16)

project(BronEngine)

add_subdirectory("vendor")
add_subdirectory("src")
target_link_libraries(Bron
	PUBLIC VendorLibrary
)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
