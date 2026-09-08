#pragma once

#include "Panels/Panel.h"

namespace bron::editor {
/// The entity tree. Owns the selection: clicking a node selects it, F2 renames it.
class SceneHierarchyPanel final : public Panel {
public:
	explicit SceneHierarchyPanel(EditorContext& context) : Panel(context) {}

	void OnImGuiRender() override;

private:
	/// The menu bar row: what can be done to the tree, rather than to one node.
	void DrawToolbar();

	/// The "Add" drop-down. Split out because it is a popup rather than part of the row.
	void DrawAddMenu();

	/// Creates an entity under the selection - or under the root when there is none - and
	/// selects it, which is what every entry of the add menu wants to do first.
	entt::entity AddEntity(const char* name);

	void DrawNode(entt::entity entity);
	void DrawRenamePopup();

	std::filesystem::path ModelPicker();
};
} // namespace bron::editor
