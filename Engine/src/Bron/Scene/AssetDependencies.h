#pragma once

#include "Bron/Scene/Scene.h"

#include <filesystem>
#include <vector>

namespace bron {
/// Every asset the scene names, as stored - relative to the asset root, deduplicated,
/// in no particular order.
///
/// It lives beside the components rather than in the exporter because it is a fact about
/// them: each component that carries a path is one case here, and keeping the two
/// together is what makes adding a component-with-a-path a single edit. Split across
/// modules, the failure mode is an export that silently omits files and a shipped game
/// that cannot load them.
///
/// It is also not only for exporting - "which assets are unused", a dependency view and
/// reloading what changed on disk are all the same question.
///
/// What it does NOT include: assets that a loader pulls in on its own. The textures an
/// .obj names through its .mtl are opened by ModelLoader from paths that live inside the
/// model file, and are never stored in the scene, so nothing here can see them. Anything
/// copying these paths has to account for a model's sidecar files - see the callers.
std::vector<std::filesystem::path> CollectAssets(const Scene& scene);
} // namespace bron
