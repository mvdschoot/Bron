cmake_minimum_required(VERSION 3.21)

project(Bron VERSION 1.0 LANGUAGES C CXX)

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
# on the working directory, which differs per IDE. See Bron/Util/Paths.h. The
# engine has no asset root of its own - its shaders are compiled into the
# library (see Bron/Graphics/BuiltinShaders.h).
set(BRON_PROJECT_ASSET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Assets" CACHE PATH "Application assets (models, icons)")

add_subdirectory(Engine)
add_subdirectory(Editor)
