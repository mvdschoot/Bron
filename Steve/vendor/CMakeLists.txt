cmake_minimum_required(VERSION 3.16)

add_subdirectory("spdlog")
add_subdirectory("glad")
add_subdirectory("glfw")
add_subdirectory("imgui")
add_subdirectory("glm")
add_subdirectory("stb")
add_subdirectory("imguizmo")
add_subdirectory("freetype")
add_subdirectory("assimp")


# INTERFACE linking because nothing is used by Vendorlibrary itself
add_library(VendorLibrary INTERFACE)
target_include_directories(VendorLibrary INTERFACE
	spdlog/include
	glad/include
	glfw/include
	glfw/deps
	glm/glm
	imgui
	stb
	imguizmo
	freetype/include
	assimp/include
)
target_link_libraries(VendorLibrary INTERFACE 
	spdlog
	glad
	glfw 
	glm
	imgui
	stb
	imguizmo
	freetype
	assimp
)

# Freetype is with vcpkg installed  
# find_package(freetype CONFIG REQUIRED)
# target_link_libraries(VendorLibrary INTERFACE freetype)
# target_include_directories(VendorLibrary INTERFACE freetype/include)

# Assimp is with vcpkg installed
# find_package(assimp CONFIG REQUIRED)
# target_link_libraries(VendorLibrary INTERFACE assimp::assimp)