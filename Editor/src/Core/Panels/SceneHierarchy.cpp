#include "SceneHierarchy.h"
#include <imgui_internal.h>

#include "ComponentRegistry.h"

#include "Core/IconManagement.h"

namespace Bron::Editor
{
	SceneHierarchyPanelData SceneHierarchyPanel::Data;

	using namespace ImGui;

	void SceneHierarchyPanel::OnAttach(Scene* scene)
	{
		Data.scene = scene;
		Data.selectedObject = entt::null;
	}

	void SceneHierarchyPanel::RenameFunction()
	{
		if (Data.selectedObject == entt::null)
			return;

		TagComponent& tag = Data.scene->reg.get<TagComponent>(Data.selectedObject);

		std::string name = "Rename '" + tag.name + "'";
		if (IsKeyPressed(ImGuiKey_F2, false))
		{
			OpenPopup(name.c_str());
		}
		ImVec2 center = GetMainViewport()->GetCenter();
		SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (BeginPopupModal(name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{

			if (!IsAnyItemActive() && !IsMouseClicked(0))
				SetKeyboardFocusHere(0);

			static char buf[256] = "";
			bool text = InputText("Name", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue);

			if (text || Button("OK", ImVec2(120, 0)))
			{
				tag.name = std::string(buf);
				CloseCurrentPopup();
			}
			SetItemDefaultFocus();
			SameLine();
			if (Button("Cancel", ImVec2(120, 0))) { CloseCurrentPopup(); }

			EndPopup();
		}
	}

	void SceneHierarchyPanel::OnImguiRender()
	{
		Begin("Scene Hierarchy");

		TreeNode(Data.scene->root);

		RenameFunction();

		PropertiesPanel();

		End();
	}

	void SceneHierarchyPanel::PropertiesPanel()
	{
		Begin("Properties");

		const entt::entity entity = Data.selectedObject;
		if (entity == entt::null)
		{
			End();
			return;
		}

		Scene& scene = *Data.scene;

		for (const ComponentMeta& component : ComponentRegistry::All())
		{
			if (!component.has(scene, entity))
				continue;

			PushID(component.name);

			// AllowOverlap lets the remove button below claim clicks in the part of the header row it
			// covers; without it the header is submitted first and swallows them into a collapse toggle.
			const bool open = CollapsingHeader(component.name,
											   ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);

			// Remove button on the header row itself, so a collapsed section can still be removed.
			bool removeRequested = false;
			if (component.remove)
			{
				SameLine(GetWindowWidth() - 30.0f);
				removeRequested = SmallButton("x");
			}

			if (open)
				component.draw(scene, entity);

			PopID();

			// Applied after drawing, so this frame's draw still sees a live component.
			if (removeRequested)
				component.remove(scene, entity);
		}

		Separator();

		if (Button("Add Component"))
			OpenPopup("AddComponent");

		if (BeginPopup("AddComponent"))
		{
			bool anyOffered = false;
			for (const ComponentMeta& component : ComponentRegistry::All())
			{
				if (!component.add || component.has(scene, entity))
					continue;

				anyOffered = true;
				if (MenuItem(component.name))
					component.add(scene, entity);
			}

			if (!anyOffered)
				TextDisabled("Nothing left to add");

			EndPopup();
		}

		End();
	}



	void SceneHierarchyPanel::TreeNode(const entt::entity entity)
	{
		entt::registry& reg = Data.scene->reg;

		ImGuiTreeNodeFlags entity_base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick;

		ImGuiTreeNodeFlags node_flags = entity_base_flags;
		if (Data.selectedObject == entity)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		SetNextItemOpen(true);

		// Entities are recycled, so the pointer of the name is not a stable id; push the entity itself.
		PushID(static_cast<int>(static_cast<u32>(entity)));
		bool open = TreeNodeEx(reg.get<TagComponent>(entity).name.c_str(), node_flags);
		if (IsItemClicked() && !IsItemToggledOpen())
			Data.selectedObject = entity;

		if(open)
		{
			for(const entt::entity child : reg.get<HierarchyComponent>(entity).children)
			{
				TreeNode(child);
			}

			TreePop();
		}
		PopID();
	}
}
