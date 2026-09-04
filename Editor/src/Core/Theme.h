#pragma once

#include <vector>

namespace Bron::Editor
{
	/// The editor's ImGui look. Kept apart from the layer so that the colour constants do
	/// not sit in the middle of the frame logic.
	namespace Theme
	{
		/// Applies the palette named by Preferences::theme, scaled by Preferences::uiScale.
		/// Falls back to the default palette when the name is not registered.
		void Apply();

		/// Names accepted by Preferences::theme, for the preferences panel.
		const std::vector<const char*>& Names();
	}
}
