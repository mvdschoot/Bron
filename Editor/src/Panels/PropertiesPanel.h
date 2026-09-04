#pragma once

#include "Panels/Panel.h"

namespace bron::editor {
/// The inspector for the selected entity. It knows nothing about individual component
/// types - the list comes from ComponentRegistry.
class PropertiesPanel final : public Panel {
public:
	explicit PropertiesPanel(EditorContext& context) : Panel(context) {}

	void OnImGuiRender() override;

private:
	void DrawComponents(entt::entity entity);
	void DrawAddComponentMenu(entt::entity entity);
};
} // namespace bron::editor
