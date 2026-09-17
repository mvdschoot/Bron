cmake_minimum_required(VERSION 3.21)

# Bron Editor - the application built on top of the engine.
add_executable(BronRuntime
        src/App.cpp
        src/Layers/WorldLayer.cpp
        src/Layers/WorldLayer.h
)

target_include_directories(BronRuntime PUBLIC src)

target_compile_features(BronRuntime PUBLIC cxx_std_20)
set_target_properties(BronRuntime PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(BronRuntime PUBLIC Bron)
