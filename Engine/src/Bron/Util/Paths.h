#pragma once

#include <filesystem>
#include <string>

#include "Bron/Core/Core.h"

namespace bron {
// Asset locations must never depend on the process working directory: every
// IDE picks a different one (CLion uses the build dir, Visual Studio the
// project dir, a bare ./BronEditor whatever the shell happens to be in).
//
// The engine only resolves paths against a root; it does not decide what the
// root is. The application owns that and calls SetAssetRoot - in the editor
// that is Project, when a project is opened. With no project open there is no
// root, and nothing should be resolving asset paths.
//
// There is deliberately no engine asset root: the engine's own shaders are
// compiled into the binary (see Bron/Graphics/BuiltinShaders.h), so assets
// belong to the application alone.
namespace paths {
// The directory every asset path stored in a scene file is relative to.
// Empty until the application sets one.
const std::filesystem::path& AssetRoot();
void SetAssetRoot(const std::filesystem::path& root);

[[nodiscard]] bool HasAssetRoot();

// Absolute location of a path stored relative to the asset root. An absolute
// input is returned unchanged, because joining it onto the root is a no-op.
std::filesystem::path Resolve(const std::filesystem::path& relative);

// Convenience wrapper for the many APIs that still take a std::string.
std::string ResolveString(const std::filesystem::path& relative);

// The inverse of Resolve: how an absolute location should be stored so that the
// save file survives the project being moved. Paths outside the root cannot be
// expressed relative to it and are returned unchanged.
std::filesystem::path Relative(const std::filesystem::path& absolute);
} // namespace paths
} // namespace bron
