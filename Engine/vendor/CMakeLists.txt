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
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)


# --------------------
# ImGui (docking branch)
#
# Upstream ships no CMakeLists, so we fetch the sources and define the target
# here. imgui_extra/ holds the few headers we add on top (imgui_spectrum.h).
# --------------------
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG 63f00dae6b9142c2570aaf30b58932153e796567 # docking
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp

        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_extra
)

target_link_libraries(imgui PUBLIC glfw)
target_compile_features(imgui PUBLIC cxx_std_17)


# --------------------
# stb
# --------------------
add_subdirectory(stb)


# --------------------
# ImGuizmo
#
# Upstream does not link imgui unless it is the top-level project, so we wire
# that up ourselves after populating it.
# --------------------
FetchContent_Declare(
        ImGuizmo
        GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
        GIT_TAG 18cef5e031d8c6973d80284c67f60549fafd78c1
)
FetchContent_MakeAvailable(ImGuizmo)

target_link_libraries(imguizmo PUBLIC imgui)


# --------------------
# Freetype
# --------------------
FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG VER-2-14-3
)
FetchContent_MakeAvailable(freetype)


# --------------------
# GLM
# --------------------
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)


# --------------------
# ASSIMP
# --------------------
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

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
# nlohmann::json
# --------------------

# The git repo carries ~1GB of test-data history, so fetch the release
# archive instead of cloning.
FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
        URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
)
FetchContent_MakeAvailable(nlohmann_json)


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
        nlohmann_json
)
