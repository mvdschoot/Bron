#pragma once

#include <filesystem>

#include "Bron/Core/Core.h"

namespace bron {
// Locations must never depend on the process working directory: every IDE picks a
// different one (CLion uses the build dir, Visual Studio the project dir, a bare
// ./BronEditor whatever the shell happens to be in).
//
// The engine only resolves paths against a root; it does not decide what the roots
// are. The application owns that and calls SetRoots - in the editor that is Project,
// when a project is opened. With no project open there are no roots, and nothing
// should be resolving anything.
//
// There is deliberately no engine root: the engine's own shaders are compiled into
// the binary (see Bron/Graphics/BuiltinShaders.h), so files belong to the
// application alone.
namespace paths {
// The directory holding the project file. Everything the project owns lives under
// it: its assets, and the per-user session state kept beside them.
const std::filesystem::path& ProjectRoot();

// The directory every asset path stored in a scene file is relative to. Normally a
// directory under the project root, though a project may point it elsewhere.
const std::filesystem::path& AssetRoot();

// Set together, because there is no meaningful state with one root and not the other.
void SetRoots(const std::filesystem::path& project, const std::filesystem::path& assets);

[[nodiscard]] bool HasRoots();

// Absolute location of a path stored relative to a root. An absolute input is
// returned unchanged, because joining it onto a root is a no-op.
std::filesystem::path ResolveProject(const std::filesystem::path& relative);
std::filesystem::path ResolveAsset(const std::filesystem::path& relative);

// The inverse of ResolveAsset: how an absolute location should be stored so that the
// save file survives the project being moved. Paths outside the root cannot be
// expressed relative to it and are returned unchanged.
std::filesystem::path RelativeToAsset(const std::filesystem::path& absolute);
} // namespace paths
} // namespace bron
