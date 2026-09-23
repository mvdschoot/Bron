#pragma once

#include "Bron/Scene/Scene.h"

#include <filesystem>
#include <vector>

namespace bron {
/// Every file the scene needs, relative to the asset root, deduplicated: the source file
/// of each asset its components point at, the textures its materials use, and the .meta
/// next to each of them - a shipped game resolves handles through those just like the
/// editor does. Built-in and memory-only assets have no file and are left out.
///
/// It lives beside the components rather than in the exporter because it is a fact about
/// them: each component that refers to an asset is one case here, and keeping the two
/// together is what makes adding such a component a single edit. Split across modules,
/// the failure mode is an export that silently omits files and a shipped game that cannot
/// load them.
///
/// It is also not only for exporting - "which assets are unused", a dependency view and
/// reloading what changed on disk are all the same question.
std::vector<std::filesystem::path> CollectAssets(const Scene& scene);
} // namespace bron
