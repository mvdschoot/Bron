cmake_minimum_required(VERSION 3.21)

# Bron Editor - the application built on top of the engine.
add_executable(BronEditor
	src/Core/App.cpp
	src/Core/AppLayer.cpp
	src/Core/EditorLayer.cpp
	src/Core/IconManagement.cpp

	src/Core/Panels/SceneHierarchy.cpp
	src/Core/Panels/ComponentRegistry.cpp
)

target_include_directories(BronEditor PUBLIC src)

target_compile_features(BronEditor PUBLIC cxx_std_20)
set_target_properties(BronEditor PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(BronEditor PUBLIC Bron)

# Makes F5 in Visual Studio start the editor instead of ALL_BUILD.
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT BronEditor)
