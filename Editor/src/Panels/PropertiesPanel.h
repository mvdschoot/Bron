#pragma once

#include "Panels/Panel.h"

namespace bron::editor {
/// The inspector for the selected entity. It knows nothing about individual component
/// types - the list comes from ComponentRegistry.
class PropertiesPanel final : public Panel {
public:
	PropertiesPanel(EditorContext& context, const std::string& name, const std::string& display_name) :
		Panel(context, name, display_name) {}

	void ImGuiContent() override;

private:
	void DrawComponents(entt::entity entity);
	void DrawAddComponentMenu(entt::entity entity);
};
} // namespace bron::editor
