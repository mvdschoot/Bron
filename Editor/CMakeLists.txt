cmake_minimum_required(VERSION 3.21)

# Bron Editor - the application built on top of the engine.
add_executable(BronEditor
	src/App.cpp

	src/Core/EditorLayer.cpp
	src/Core/Preferences.cpp
	src/Core/EditorCamera.cpp
	src/Core/Icons.cpp
	src/Core/Theme.cpp
	src/Core/Project.cpp
	src/Core/Exporter.cpp

	src/Panels/ViewportPanel.cpp
	src/Panels/SceneHierarchyPanel.cpp
	src/Panels/PropertiesPanel.cpp
	src/Panels/ProjectPanel.cpp
	src/Panels/StatisticsPanel.cpp
	src/Panels/PreferencesPanel.cpp
	src/Panels/ComponentRegistry.cpp
        src/Panels/FileExplorerPanel.cpp
        src/Panels/FileExplorerPanel.h
)

target_include_directories(BronEditor PUBLIC src)

# The editor's own icons, built into the binary. Adding one is: drop a white-on-
# transparent PNG in resources/icons (Editor/resources/icons/generate.py draws the set),
# name it here, and add the matching Id to src/Core/Icons.h - the symbol is the file
# name, so 'add.png' arrives as embedded::kAdd.
include(${CMAKE_SOURCE_DIR}/cmake/EmbedResources.cmake)

bron_embed_resources(BronEditor
	NAMESPACE bron::editor::embedded
	BASENAME EditorIcons
	FILES
		resources/icons/add.png
		resources/icons/delete.png
		resources/icons/save.png
		resources/icons/up.png
		resources/icons/folder.png
		resources/icons/file.png
		resources/icons/model.png
		resources/icons/scene.png
		resources/icons/project.png
)

target_compile_features(BronEditor PUBLIC cxx_std_20)
set_target_properties(BronEditor PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(BronEditor PUBLIC Bron)

# Makes F5 in Visual Studio start the editor instead of ALL_BUILD.
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT BronEditor)

