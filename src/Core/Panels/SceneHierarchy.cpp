#include "SceneHierarchy.h"
#include <imgui_internal.h>

#include "Core/IconManagement.h"

namespace Steve
{
	SceneHierarchyPanelData SceneHierarchyPanel::Data;

	using namespace ImGui;

	// Euler angles for the currently selected entity. Cached because converting a quaternion to euler
	// angles is lossy: recomputing it every frame makes a rotation drag jump around near the poles.
	static entt::entity sEulerCacheOwner = entt::null;
	static glm::vec3 sEulerCache{0.0f};

	void SceneHierarchyPanel::OnAttach(Scene* scene)
	{
		Data.scene = scene;
		Data.selectedObject = entt::null;
		InvalidateEulerCache();
	}

	void SceneHierarchyPanel::InvalidateEulerCache()
	{
		sEulerCacheOwner = entt::null;
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

		entt::registry& reg = Data.scene->reg;

		if (TransformComponent* t = reg.try_get<TransformComponent>(entity);
			t && CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// Dragger position
			DragFloat3("Position", value_ptr(t->Position));

			// Dragger rotation. The cache is only refreshed when the selection changes, so that dragging
			// through a pole does not send the angles somewhere else mid-drag.
			if (sEulerCacheOwner != entity)
			{
				sEulerCacheOwner = entity;
				sEulerCache = glm::degrees(glm::eulerAngles(t->RotationQuat));
			}

			if (DragFloat3("Rotation", value_ptr(sEulerCache)))
			{
				t->RotationQuat = glm::quat(glm::radians(sEulerCache));
			}

			// Dragger scaling
			DragFloat3("Scaling", value_ptr(t->Scaling));
		}

		if (PointLightComponent* light = reg.try_get<PointLightComponent>(entity);
			light && CollapsingHeader("Light settings", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ColorEdit3("Color", value_ptr(light->color)))
			{
				Data.scene->lightManagement.MarkDirty();
			}
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
		{
			Data.selectedObject = entity;
			InvalidateEulerCache();
		}

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
