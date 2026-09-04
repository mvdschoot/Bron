#pragma once

#include "Panels/Panel.h"

namespace bron::editor {
/// Editor for the application-scope Preferences. Closed by default and opened from the
/// menu bar, since it is not a window anyone keeps docked.
class PreferencesPanel final : public Panel {
public:
	explicit PreferencesPanel(EditorContext& context) : Panel(context) {}

	void OnImGuiRender() override;

	void Open() { open_ = true; }

private:
	bool open_ = false;
};
} // namespace bron::editor
