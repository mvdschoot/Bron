#include "SceneHierarchy.h"
#include <imgui_internal.h>

#include "Core/IconManagement.h"

namespace Steve
{
	SceneHierarchyPanelData SceneHierarchyPanel::Data;

	using namespace ImGui;

	void SceneHierarchyPanel::OnAttach(Scene* scene)
	{
		Data.scene = scene;
		Data.selectedObject = nullptr;
	}


	void SceneHierarchyPanel::RenameFunction()
	{
		if (Data.selectedObject == nullptr)
			return;
		std::string name = "Rename '" + Data.selectedObject->name + "'";
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
				Data.selectedObject->name = std::string(buf);
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

		Node* ent = Data.selectedObject;
		if (ent == nullptr)
		{
			End();
			return;
		}

		if (ent->Contains<TransformComponent>() && CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			TransformComponent& t = *ent->GetComponent<TransformComponent>();

			// Dragger position
			DragFloat3("Position", value_ptr(t.Position));

			// Dragger rotation
			t.SyncEulerFromQuat();
			if (DragFloat3("Rotation", value_ptr(t.EulerCache))) {
				t.SyncQuatFromEuler();
			}

			// Dragger scaling
			DragFloat3("Scaling", value_ptr(t.Scaling));
		}

		if (ent->type & NodeType_PointLight && CollapsingHeader("Light settings", ImGuiTreeNodeFlags_DefaultOpen))
		{
			glm::vec3 color = ((PointLight*)ent)->getColor();
			ColorEdit3("Color", value_ptr(color));
			((PointLight*)ent)->setColor(color);
		}

		End();
	}



	void SceneHierarchyPanel::TreeNode(Node* node)
	{
		if (SkipNode(node))
			return;

		ImGuiTreeNodeFlags entity_base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick;

		ImGuiTreeNodeFlags node_flags = entity_base_flags;
		if (Data.selectedObject == node)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		SetNextItemOpen(true);
		bool open = TreeNodeEx(node->name.c_str(), node_flags);
		if (IsItemClicked() && !IsItemToggledOpen())
			Data.selectedObject = node;


		float height = GetTextLineHeight();

		// SameLine();
		// ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(node->name.c_str()).x
		// 	- ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);


		//
		// u32 id = Icons::GetIconId("add");
		// if(ImageButton(reinterpret_cast<void*>(id), ImVec2{ height,height }))
		// {
		// 	OpenPopup("Node options");
		// }
		//
		//
		// if (BeginPopupContextWindow("Node options"))
		// {
		// 	if (MenuItem("Add rigidBody"))
		// 	{
		// 		// Data.scene->
		// 	}
		// 	if (MenuItem("Close"))
		// 	{
		// 		CloseCurrentPopup();
		// 	}
		// 	EndPopup();
		// }

		if(open)
		{
			for(Node* n : node->children)
			{
				TreeNode(n);
			}

			TreePop();
		}
	}

	bool SceneHierarchyPanel::SkipNode(Node* entity)
	{
		u32 type = entity->type;
		if (type & NodeType_Mesh && type & NodeType_PointLight)
			return true;
		return false;
	}
}
