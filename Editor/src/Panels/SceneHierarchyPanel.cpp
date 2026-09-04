#include "Panels/SceneHierarchyPanel.h"

#include <imgui_internal.h>

namespace bron::editor {
using namespace ImGui;

void SceneHierarchyPanel::OnImGuiRender() {
	Begin("Scene Hierarchy");

	if (!context_.HasScene()) {
		TextDisabled("No scene open.");
		End();
		return;
	}

	DrawNode(context_.active_scene->root);
	DrawRenamePopup();

	End();
}

void SceneHierarchyPanel::DrawNode(const entt::entity entity) {
	entt::registry& reg = context_.active_scene->reg;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (context_.selection == entity)
		flags |= ImGuiTreeNodeFlags_Selected;

	SetNextItemOpen(true);

	// Entities are recycled, so the pointer of the name is not a stable id; push the entity itself.
	PushID(static_cast<int>(static_cast<u32>(entity)));

	const bool open = TreeNodeEx(reg.get<TagComponent>(entity).name.c_str(), flags);
	if (IsItemClicked() && !IsItemToggledOpen())
		context_.selection = entity;

	if (open) {
		for (const entt::entity child: reg.get<HierarchyComponent>(entity).children)
			DrawNode(child);

		TreePop();
	}

	PopID();
}

void SceneHierarchyPanel::DrawRenamePopup() {
	if (!context_.HasSelection())
		return;

	TagComponent& tag = context_.active_scene->reg.get<TagComponent>(context_.selection);

	const std::string title = "Rename '" + tag.name + "'";
	if (IsKeyPressed(ImGuiKey_F2, false))
		OpenPopup(title.c_str());

	const ImVec2 center = GetMainViewport()->GetCenter();
	SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (!BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		return;

	if (!IsAnyItemActive() && !IsMouseClicked(0))
		SetKeyboardFocusHere(0);

	static char buffer[256] = "";
	const bool committed = InputText("Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue);

	if (committed || Button("OK", ImVec2(120, 0))) {
		tag.name = buffer;
		CloseCurrentPopup();
	}

	SetItemDefaultFocus();
	SameLine();
	if (Button("Cancel", ImVec2(120, 0)))
		CloseCurrentPopup();

	EndPopup();
}
} // namespace bron::editor
