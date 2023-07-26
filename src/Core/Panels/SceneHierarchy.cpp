#include "SceneHierarchy.h"

namespace Steve
{
	SceneHierarchyPanelData SceneHierarchyPanel::Data;

	void SceneHierarchyPanel::OnAttach(Scene* scene)
	{
		Data.scene = scene;
		Data.selected = nullptr;
	}


	void SceneHierarchyPanel::RenameFunction()
	{
		if (Data.selected == nullptr)
			return;
		std::string name = "Rename '" + Data.selected->Name + "'";
		if (ImGui::IsKeyPressed(ImGuiKey_F2, false))
		{
			ImGui::OpenPopup(name.c_str());
		}
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal(name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{

			if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
				ImGui::SetKeyboardFocusHere(0);

			static char buf[256] = "";
			bool text = ImGui::InputText("Name", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue);

			if (text || ImGui::Button("OK", ImVec2(120, 0)))
			{
				Data.selected->Name = std::string(buf);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }

			ImGui::EndPopup();
		}
	}

	void SceneHierarchyPanel::OnImguiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		ImGuiTreeNodeFlags entity_base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		ImGui::SetNextItemOpen(true);
		if (ImGui::TreeNode("Models"))
		{
			for (int x = 0; x < Data.scene->AllModels.size(); x++)
			{
				ImGuiTreeNodeFlags node_flags = entity_base_flags;
				if (Data.selected == Data.scene->AllModels[x])
				{
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}

				bool open = ImGui::TreeNodeEx((void*)(intptr_t)x, node_flags | ImGuiTreeNodeFlags_Framed, Data.scene->AllModels[x]->Name.c_str());
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					Data.selected = Data.scene->AllModels[x];

				// if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
				// {
				// 	ImGui::OpenPopup("Item options");
				// 	Data.selected = Data.scene->AllModels[x];
				// }

				ImGui::PushID(x);
				if (ImGui::BeginPopupContextWindow())
				{
					if (ImGui::MenuItem("Add rigid body"))
					{

					}
					if (ImGui::MenuItem("Weg"))
					{
						// if (ImGui::IsItemHovered())
						// {
						// 	ImGui::OpenPopup("Add to item");
						// }
					}
					ImGui::EndPopup();
				}
				ImGui::PopID();

				if (open)
				{
					for (int y = 0; y < Data.scene->AllModels[x]->Meshes.size(); y++)
					{
						ImGuiTreeNodeFlags node_flags = entity_base_flags | ImGuiTreeNodeFlags_Leaf;
						if (Data.selected == Data.scene->AllModels[x]->Meshes[y])
						{
							node_flags |= ImGuiTreeNodeFlags_Selected;
						}

						ImGui::TreeNodeEx((void*)(intptr_t)y, node_flags, Data.scene->AllModels[x]->Meshes[y]->Name.c_str());
						if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
							Data.selected = Data.scene->AllModels[x]->Meshes[y];

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
			}
			ImGuiTreeNodeFlags node_flags = entity_base_flags;

			ImGui::TreePop();
		}

		ImGui::Separator();

		ImGui::SetNextItemOpen(true);
		if (ImGui::TreeNode("Point lights"))
		{
			if (ImGui::Button("Add pointlight")) {
				Data.scene->AddPointLight(glm::vec3(0.0), glm::vec3(1.0));
			}

			for (int x = 0; x < Data.scene->PointLights.size(); x++)
			{
				ImGuiTreeNodeFlags node_flags = entity_base_flags | ImGuiTreeNodeFlags_Leaf;
				if (Data.selected == Data.scene->PointLights[x])
				{
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}

				ImGui::TreeNodeEx((void*)(intptr_t)x, node_flags, Data.scene->PointLights[x]->Name.c_str());
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					Data.selected = Data.scene->PointLights[x];
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		RenameFunction();

		PropertiesPanel();

		ImGui::End();
	}

	void SceneHierarchyPanel::PropertiesPanel()
	{

		ImGui::Begin("Properties");

		Entity* ent = Data.selected;
		if (ent == nullptr)
		{
			ImGui::End();
			return;
		}

		std::string header = "";
		std::vector<std::string> types = ent->TypeToString();
		for(int x = 0; x < types.size(); x++)
		{
			if (x > 0)
				header += " & ";
			header += types[x];
		}

		ImGui::TextColored(ImColor(128,128,128,255), "%s: '%s'", header.c_str(), ent->Name.c_str());
		ImGui::Separator();

		if (ent->Contains<TransformComponent>() && ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			TransformComponent& t = *ent->GetComponent<TransformComponent>();

			ImGui::DragFloat3("Position", value_ptr(t.Position));
			ImGui::DragFloat3("Rotation", value_ptr(t.Rotation));
			ImGui::DragFloat3("Scaling", value_ptr(t.Scaling));
			
			if (ent->Type & POINTLIGHT_ENTITY)
			{
				((PointLight*)ent)->SetUniformPosition(t.Position);
			}
		}

		if (ent->Type & POINTLIGHT_ENTITY && ImGui::CollapsingHeader("Light settings", ImGuiTreeNodeFlags_DefaultOpen))
		{
			glm::vec3 color = ((PointLight*)ent)->GetColor();
			ImGui::ColorEdit3("Color", value_ptr(color));
			((PointLight*)ent)->SetColor(color);
		}

		ImGui::End();
	}
}
