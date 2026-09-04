#pragma once

#include "Panels/Panel.h"

namespace bron::editor {
/// The entity tree. Owns the selection: clicking a node selects it, F2 renames it.
class SceneHierarchyPanel final : public Panel {
public:
	explicit SceneHierarchyPanel(EditorContext& context) : Panel(context) {}

	void OnImGuiRender() override;

private:
	void DrawNode(entt::entity entity);
	void DrawRenamePopup();
};
} // namespace bron::editor
