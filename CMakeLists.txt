cmake_minimum_required(VERSION 3.21)

project(LuigiMaker VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Single-config generators (Ninja, Unix Makefiles) have no build type by
# default, which silently gives you an unoptimised, undefined-NDEBUG build.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# Everything lands in one place regardless of generator, so the DLLs Windows
# needs at runtime end up next to the executable.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# The application asset root is baked into the binary so that nothing depends
# on the working directory, which differs per IDE. See Steve/Util/Paths.h. The
# engine has no asset root of its own - its shaders are compiled into the
# library (see Steve/Graphics/BuiltinShaders.h).
set(STEVE_PROJECT_ASSET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Assets" CACHE PATH "Application assets (models, icons)")

add_subdirectory(Steve)

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

target_compile_features(LuigiMaker PUBLIC cxx_std_20)
set_target_properties(LuigiMaker PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(LuigiMaker PUBLIC Steve)
target_link_libraries(LuigiMaker PUBLIC VendorLibrary)

# Makes F5 in Visual Studio start the app instead of ALL_BUILD.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LuigiMaker)
