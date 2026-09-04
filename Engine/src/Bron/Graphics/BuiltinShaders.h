#pragma once

#include <string>

namespace bron {
// The shaders behind the engine's built-in renderers. They are not assets:
// each one has a hard contract with the C++ that drives it (vertex layout,
// uniform names, texture slots), so the engine owns them and keeps them in
// its own translation unit. Nothing is read from disk, and there is no
// engine asset folder to ship or copy next to the executable.
namespace builtin_shaders {
enum class Id { kRenderer2D, kGrid, kPhong3D, kText };

// GLSL source for a built-in shader, ready for Shader::Create.
std::string Source(Id id);
} // namespace builtin_shaders
} // namespace bron
