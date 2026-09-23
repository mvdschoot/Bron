#include "Panels/SceneHierarchyPanel.h"

#include "nfd.hpp"

#include <imgui_internal.h>

#include "Core/Icons.h"

namespace bron::editor {
using namespace ImGui;

namespace {
// Shared by the button that opens the drop-down and the popup itself, which have to agree
// on it. "##" keeps it out of the drawn menu.
constexpr const char* kAddMenu = "##add.entity";
} // namespace

void SceneHierarchyPanel::OnImGuiRender() {
	Begin("Scene Hierarchy", nullptr, ImGuiWindowFlags_MenuBar);

	if (!context_.HasScene()) {
		TextDisabled("No scene open.");
		End();
		return;
	}

	DrawToolbar();

	DrawNode(context_.active_scene->root);
	DrawRenamePopup();

	End();
}

void SceneHierarchyPanel::DrawToolbar() {
	Scene& scene = *context_.active_scene;

	BeginMenuBar();

	if (icons::Button(icons::Id::kAdd, "Add an entity, under the selected one"))
		OpenPopup(kAddMenu);

	DrawAddMenu();

	// The root is the scene itself; deleting it would leave nowhere to put anything.
	const bool deletable = context_.HasSelection() && context_.selection != scene.root;

	BeginDisabled(!deletable);
	if (icons::Button(icons::Id::kDelete, "Delete the selected entity and its children")) {
		scene.DestroyEntity(context_.selection);
		context_.ClearSelection();
	}
	EndDisabled();

	EndMenuBar();
}

void SceneHierarchyPanel::DrawAddMenu() {
	// BeginMenu() draws its own label and only takes text, so an icon cannot be its title.
	// The drop-down is therefore opened by hand: the icon button above calls OpenPopup, and
	// the popup is pinned under that button so it still behaves like the menu it looks like.
	// The position is set every frame - SetNextWindowPos only applies to the Begin that
	// follows it, and the button it is measured from has just been drawn.
	SetNextWindowPos(ImVec2(GetItemRectMin().x, GetItemRectMax().y));

	if (!BeginPopup(kAddMenu))
		return;

	if (MenuItem("Empty Entity"))
		AddEntity("Entity");

	if (MenuItem("Point Light")) {
		const entt::entity light = AddEntity("Point Light");
		context_.active_scene->reg.emplace<PointLightComponent>(light);
	}

	if (MenuItem("Model")) {
		std::filesystem::path path = ModelPicker();
		if (!path.empty()) {
			const entt::entity model = context_.active_scene->CreateModel(path);
			if (model != entt::null) {
				const entt::entity parent = context_.HasSelection() ? context_.selection : context_.active_scene->root;
				context_.active_scene->AddChild(parent, model);
			}
		}
	}

	EndPopup();
}

entt::entity SceneHierarchyPanel::AddEntity(const char* name) {
	Scene& scene = *context_.active_scene;

	// Under the selection, so building a hierarchy does not mean creating at the root
	// and dragging afterwards. With nothing selected the root is the parent.
	const entt::entity parent = context_.HasSelection() ? context_.selection : scene.root;

	context_.selection = scene.CreateEntity(name, parent);

	return context_.selection;
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

	if (IsItemHovered() && IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
		const glm::vec3& position = context_.active_scene->reg.get<TransformComponent>(context_.selection).Position;
		context_.camera.Focus(position);
	}

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

std::filesystem::path SceneHierarchyPanel::ModelPicker() {
	std::filesystem::path file;
	if (NFD::Init()) {
		NFD::UniquePath out_path;
		nfdfilteritem_t filter_item[1] = {{"Model files", "glb"}};
		if (NFD::OpenDialog(out_path, filter_item, 1, nullptr) == NFD_OKAY) {
			file = out_path.get();
		}
		NFD_Quit();
	}
	return file;
}
} // namespace bron::editor
