cmake_minimum_required(VERSION 3.21)

# Bron Runtime - what a game ships as. The engine plus a boot sequence, and none of the
# editor: no panels, no project, no authoring. What it runs is decided by the manifest
# the exporter writes next to the executable.
add_executable(BronRuntime
        src/App.cpp

        src/Layers/WorldLayer.cpp
)

target_include_directories(BronRuntime PUBLIC src)

target_compile_features(BronRuntime PUBLIC cxx_std_20)
set_target_properties(BronRuntime PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(BronRuntime PUBLIC Bron)
