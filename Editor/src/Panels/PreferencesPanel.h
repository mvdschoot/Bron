#pragma once

#include "Panels/Panel.h"

namespace Bron::Editor
{
	/// Editor for the application-scope Preferences. Closed by default and opened from the
	/// menu bar, since it is not a window anyone keeps docked.
	class PreferencesPanel final : public Panel
	{
	public:
		explicit PreferencesPanel(EditorContext& context) : Panel(context) {}

		void OnImGuiRender() override;

		void Open() { mOpen = true; }

	private:
		bool mOpen = false;
	};
}
