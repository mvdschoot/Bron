include_directories(.)

add_library(stb stb_image.cpp)

target_include_directories(stb PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)