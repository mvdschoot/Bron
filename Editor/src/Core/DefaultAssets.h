#pragma once

#include "Bron/Scene/Asset.h"

#include <filesystem>

namespace bron::editor::defaults {
/// The assets the editor gives every project, so that a new text has a font before the
/// user has imported one. They are ordinary files in the project, not something the
/// engine knows about: a scene saves them by handle like any other asset, the exporter
/// copies them, and the runtime loads them without knowing they came from the editor.
///
/// The files live under <asset root>/Bron/. Their handles are fixed, so a scene made in
/// one project still finds them in another.
inline const assets::AssetHandle kFont = assets::FixedHandle("editor-font-default");

/// Writes each default asset that is missing from the project, and registers it under
/// its fixed handle. One the user has changed is left alone. Runs after
/// AssetManager::Refresh(): before it, the registry still holds the previous project,
/// and would answer for a path both projects have.
void Seed(const std::filesystem::path& asset_root);
} // namespace bron::editor::defaults
