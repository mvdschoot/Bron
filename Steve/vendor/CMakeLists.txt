cmake_minimum_required(VERSION 3.16)
project(VendorLibraries)

include(FetchContent)

add_library(VendorLibrary INTERFACE)


# --------------------
# SPDLOG
# --------------------
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.3
)
FetchContent_MakeAvailable(spdlog)


# --------------------
# GLAD
# --------------------
add_subdirectory(glad)


# --------------------
# GLFW
# --------------------
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)


# --------------------
# ImGui
# --------------------
add_subdirectory(imgui)


# --------------------
# stb
# --------------------
add_subdirectory(stb)


# --------------------
# ImGuizmo
# --------------------
add_subdirectory(ImGuizmo)


# --------------------
# Freetype
# --------------------
FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG VER-2-13-3
)
FetchContent_MakeAvailable(freetype)


# --------------------
# GLM
# --------------------
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG master
)
FetchContent_MakeAvailable(glm)


# --------------------
# ASSIMP
# --------------------
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v6.0.2
)
FetchContent_MakeAvailable(assimp)


# --------------------
# magic_enum
# --------------------
FetchContent_Declare(
        magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG v0.9.7
)
FetchContent_MakeAvailable(magic_enum)


# --------------------
# EnTT
# --------------------
FetchContent_Declare(
        EnTT
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.16.0
)
FetchContent_MakeAvailable(EnTT)


# --------------------
# Vendor library
# --------------------
target_link_libraries(VendorLibrary INTERFACE
        spdlog
        glad
        glfw
        imgui
        imguizmo
        freetype
        assimp
        stb
        glm
        magic_enum
        EnTT::EnTT
)