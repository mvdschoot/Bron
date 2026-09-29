# Bron

**A C++20 / OpenGL game engine with a full editor, Lua scripting, and a standalone runtime you can export games to.**

[![CI](https://github.com/mvdschoot/Bron/actions/workflows/ci.yml/badge.svg)](https://github.com/mvdschoot/Bron/actions/workflows/ci.yml)
![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)
![OpenGL 4.5](https://img.shields.io/badge/OpenGL-4.5-5586A4?logo=opengl&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.21%2B-064F8C?logo=cmake&logoColor=white)
![Platforms](https://img.shields.io/badge/platforms-Windows%20%7C%20Linux-lightgrey)

## Demo

<!-- To play inline: edit this file on github.com, drag docs/demo.mp4 onto this line,
     and GitHub replaces it with a github.com/user-attachments/... link. -->

[▶ Watch the demo](docs/demo.mp4)

## Features

### Rendering

- **Renderer abstraction**: an API-agnostic `API` / `Command` layer with an OpenGL 4.5 backend in `Platform/OpenGL`.
- **Phong lighting** with multiple point lights and diffuse / specular texture maps.
- **Batched 2D renderer** for quads and **FreeType text**, used for both the HUD and in-world labels.
- **Infinite editor grid**, drawn procedurally from a single screen quad. It picks its level of detail per pixel, fades
  with distance and writes real depth so geometry hides it.
- **Selection outlines** drawn with the stencil buffer.
- **Mouse picking** through an integer entity-ID attachment on the framebuffer.

### Scene & ECS

- Built on **EnTT**, with a parent/child hierarchy, UUIDs and world-transform propagation.
- Components: Transform, Mesh/Material, PointLight, Camera, Script, Visibility, Canvas, RectTransform, Text2D, Box2D.
- **JSON scene serialization** (nlohmann/json).
- **Asset manager** with dependency tracking, and **model import** through Assimp.

### UI system

- **Screen-space and world-space canvases**. RectTransforms are anchored to their parent's rect, like Unity's uGUI, and
  UI is stored as ordinary scene data.

### Scripting

- **Lua 5.4 through sol2**, with bindings for glm types, components, entities and input.

### Editor

- **ImGui docking editor** with viewport, scene hierarchy, properties, file explorer, project settings, statistics and
  preferences panels.
- **Transform gizmos** (ImGuizmo) and an orbit camera with zoom.
- **Play mode**: the scene is copied on Play, the copy runs, and it is thrown away on Stop, so the edited scene is never
  touched.
- **Viewport overlays**: editor-only drawing (grid, outlines) is immediate-mode and never stored in the scene, so it
  cannot leak into a game.
- **Projects** (`.brn`) that keep shared project settings separate from per-user preferences.

### Export & runtime

- **One-click export** to a standalone game folder: the prebuilt `BronRuntime` executable, every asset the scene depends
  on, and a `game.brn` manifest with only relative paths.

### Physics (work in progress)

- Only some initial structures are present, it's a WIP.
- Custom **BVH** broad phase and collision code in `Engine/src/Bron/Physics`. It is not yet connected to the ECS.

### Engineering

- **CI** on every push: MSVC, GCC and Clang builds (Debug + Release) plus a `clang-format` check.
- **CMake presets** for every platform. Every dependency is pulled with FetchContent, so there is nothing to install by
  hand.
- `.clang-format` / `.clang-tidy`, and a pre-commit hook that formats staged files.

## Architecture

```
        ┌──────────────┐        ┌──────────────┐
        │  BronEditor  │        │ BronRuntime  │
        │ ImGui panels │        │ plays an     │
        │ overlays     │        │ exported game│
        │ export       │        │              │
        └──────┬───────┘        └──────┬───────┘
               └──────────┬────────────┘
                   ┌──────┴───────┐
                   │     Bron     │  static library
                   │  renderer    │
                   │  scene / ECS │
                   │  scripting   │
                   │  platform    │
                   └──────────────┘
```

The engine knows nothing about the editor. `SceneRenderer` draws only the scene, so the editor and the runtime render a
scene identically. The editor adds its own drawing between the world pass and the HUD pass.

## Folder structure

```
Bron/
├── Engine/                  The engine, built as the static library `Bron`
│   ├── src/Bron/
│   │   ├── Core/            Application loop, window, logging, profiling, UUIDs
│   │   ├── Game/            Game manifest (game.brn) read by the runtime
│   │   ├── Graphics/        Shaders, buffers, textures, framebuffers, lights
│   │   │   └── Renderer/    Scene, world, 2D, canvas and grid renderers
│   │   ├── Input/           Events, key and mouse codes, input polling
│   │   ├── Layers/          Layer stack and the ImGui layer
│   │   ├── Physics/         BVH and collision detection (WIP)
│   │   ├── Scene/           ECS scene, components, assets, model/font loading
│   │   │   └── Serialization/
│   │   ├── Scripting/       Lua bindings (sol2)
│   │   └── Util/            Paths and helpers
│   ├── src/Platform/
│   │   ├── Desktop/         GLFW window and input
│   │   └── OpenGL/          OpenGL implementation of the renderer API
│   └── vendor/              glad, stb and the FetchContent setup for everything else
├── Editor/                  BronEditor, the editor application
│   ├── src/
│   │   ├── Core/            Editor layer, camera, project, preferences, exporter
│   │   ├── Panels/          Viewport, hierarchy, properties, file explorer, ...
│   │   └── Overlays/        Editor-only viewport drawing (grid, outlines)
│   └── resources/           Icons and default assets embedded into the binary
├── Runtime/                 BronRuntime, the player that exported games ship with
├── cmake/                   CMake scripts that embed resources into the binaries
├── tools/git-hooks/         clang-format pre-commit hook
└── docs/                    Demo video
```

## Building

**Requirements:** CMake 3.21+, Ninja, and one of MSVC (Visual Studio 2026), GCC or Clang with C++20 support.

On Linux, install GLFW's and nfd's system dependencies first:

```sh
sudo apt-get install ninja-build libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev \
  libxcursor-dev libxi-dev libxkbcommon-dev libwayland-dev wayland-protocols \
  extra-cmake-modules libgtk-3-dev
```

Then configure and build with a preset (`windows-msvc`, `windows-vs`, `linux-gcc`, `linux-clang`):

```sh
cmake --preset linux-gcc
cmake --build --preset linux-gcc-debug      # or linux-gcc-release
```

On Windows, run this from a *Developer Command Prompt* so MSVC is on the path. The executables end up in
`build/<preset>/bin/` (for multi-config generators, in a `Debug/` or `Release/` folder inside it).

To format your commits automatically:

```sh
git config core.hooksPath tools/git-hooks
```

## Dependencies

All of these are fetched automatically at configure time.

| Library                                                                        | Used for                        |
|--------------------------------------------------------------------------------|---------------------------------|
| [GLFW](https://github.com/glfw/glfw)                                           | Window, context and input       |
| [glad](https://github.com/Dav1dde/glad)                                        | OpenGL loader                   |
| [Dear ImGui](https://github.com/ocornut/imgui) (docking)                       | Editor UI                       |
| [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)                        | Transform gizmos                |
| [EnTT](https://github.com/skypjack/entt)                                       | Entity component system         |
| [glm](https://github.com/g-truc/glm)                                           | Math                            |
| [Assimp](https://github.com/assimp/assimp)                                     | Model import                    |
| [FreeType](https://freetype.org)                                               | Font rasterization              |
| [stb](https://github.com/nothings/stb)                                         | Image loading                   |
| [Lua](https://www.lua.org) + [sol2](https://github.com/ThePhD/sol2)            | Scripting                       |
| [nlohmann/json](https://github.com/nlohmann/json)                              | Scene and project serialization |
| [spdlog](https://github.com/gabime/spdlog)                                     | Logging                         |
| [nativefiledialog-extended](https://github.com/btzy/nativefiledialog-extended) | Native file dialogs             |

## Roadmap

- Build the physics engine
- Expand the editor
- Audio
- Prefabs and asset UUIDs
- Packed-asset export (single pak file, later a single executable)

## About

Bron is a personal project I have been working on, on-and-off, for about 6 years to learn how engines work, from the GPU
calls up to the editor.
