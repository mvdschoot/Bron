#pragma once

#include "Panels/Panel.h"

namespace bron::editor {
/// Editor for the application-scope Preferences. Closed by default and opened from the
/// menu bar, since it is not a window anyone keeps docked.
class PreferencesPanel final : public Panel {
public:
	PreferencesPanel(EditorContext& context, const std::string& name, const std::string& display_name) :
		Panel(context, name, display_name) {
		closable_ = true;
		open_ = false;
	}

protected:
	void ImGuiContent() override;
};
} // namespace bron::editor
