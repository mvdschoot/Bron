cmake_minimum_required(VERSION 3.16)
project(VendorLibraries)

include(FetchContent)


add_library(VendorLibrary INTERFACE)


# --------------------
# SPDLOG (header-only with optional compiled mode)
# --------------------
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.15.3
)
FetchContent_MakeAvailable(spdlog)

# --------------------
# GLAD (local)
# --------------------
add_subdirectory(glad)
target_include_directories(VendorLibrary INTERFACE glad/include)

# --------------------
# GLFW (compiled library)
# --------------------
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# --------------------
# ImGui (local)
# --------------------
add_subdirectory(imgui)
target_include_directories(VendorLibrary INTERFACE imgui)

# --------------------
# stb (header-only)
# --------------------
add_subdirectory(stb)
target_include_directories(VendorLibrary INTERFACE stb)

# --------------------
# ImGuizmo (header-only + single .cpp)
# --------------------
add_subdirectory(imguizmo)
target_include_directories(VendorLibrary INTERFACE imguizmo)

# --------------------
# Freetype (compiled library)
# --------------------
FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
  GIT_TAG VER-2-13-3
)
FetchContent_MakeAvailable(freetype)

# --------------------
# GLM (header-only)
# --------------------
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG master
)
FetchContent_MakeAvailable(glm)

# --------------------
# ASSIMP (compiled library)
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
# Vendor library (aggregates all dependencies)
# --------------------

# set_target_properties(VendorLibrary PROPERTIES LINKER_LANGUAGE CXX)


# Compiled libraries are PUBLIC so they link transitively
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
)