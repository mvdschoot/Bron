#pragma once

namespace Bron::Editor
{
	/// The editor's ImGui look. Kept apart from the layer so that the ~50 lines of colour
	/// constants do not sit in the middle of the frame logic.
	namespace Theme
	{
		void Apply();
	}
}
