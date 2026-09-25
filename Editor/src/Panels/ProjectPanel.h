#pragma once

#include "Panels/Panel.h"

namespace bron::editor {
/// Shows the open project and lets the scene be written and read. The asset browser will
/// grow here; for now it is the project's settings plus the scene it starts with.
class ProjectPanel final : public Panel {
public:
	ProjectPanel(EditorContext& context, const std::string& name, const std::string& display_name) :
		Panel(context, name, display_name) {}

	void ImGuiContent() override;

private:
	void DrawSettings();
};
} // namespace bron::editor
