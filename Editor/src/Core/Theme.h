#pragma once

#include <vector>

namespace bron::editor {
/// The editor's ImGui look. Kept apart from the layer so that the colour constants do
/// not sit in the middle of the frame logic.
namespace theme {
/// Applies the palette named by Preferences::theme, scaled by Preferences::ui_scale.
/// Falls back to the default palette when the name is not registered.
void Apply();

/// Names accepted by Preferences::theme, for the preferences panel.
const std::vector<const char*>& Names();
} // namespace theme
} // namespace bron::editor
