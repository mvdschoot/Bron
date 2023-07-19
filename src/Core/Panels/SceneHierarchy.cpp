#include "SceneHierarchy.h"

namespace Steve
{
	struct SceneHierarchyPanelData
	{
		graphics::Scene* scene;
		Entity* selected;
	};

	static SceneHierarchyPanelData scData;

	void SceneHierarchyPanel::OnAttach(graphics::Scene* scene)
	{
		scData.scene = scene;
		scData.selected = nullptr;
	}


	void SceneHierarchyPanel::RenameFunction()
	{
		if (scData.selected == nullptr)
			return;
		std::string name = "Rename '" + scData.selected->Name + "'";
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
				scData.selected->Name = std::string(buf);
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
		using namespace graphics;

		ImGui::Begin("Scene Hierarchy");
		ImGuiTreeNodeFlags entity_base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		ImGui::SetNextItemOpen(true);
		if (ImGui::TreeNode("Models"))
		{
			for (int x = 0; x < scData.scene->AllModels.size(); x++)
			{
				ImGuiTreeNodeFlags node_flags = entity_base_flags;
				if (scData.selected == scData.scene->AllModels[x])
				{
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}

				bool open = ImGui::TreeNodeEx((void*)(intptr_t)x, node_flags, scData.scene->AllModels[x]->Name.c_str());
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					scData.selected = scData.scene->AllModels[x];

				if (open)
				{
					if (ImGui::CollapsingHeader("Properties"))
					{
						TransformComponent& t = *scData.scene->AllModels[x]->GetComponent<TransformComponent>();
						
						ImGui::DragFloat3("Position", glm::value_ptr(t.Position));
						ImGui::DragFloat3("Rotation", glm::value_ptr(t.Rotation), 1, -180, 180);
						ImGui::DragFloat3("Scaling", glm::value_ptr(t.Scaling));
						
					}

					for (int y = 0; y < scData.scene->AllModels[x]->Meshes.size(); y++)
					{
						ImGuiTreeNodeFlags node_flags = entity_base_flags;
						if (scData.selected == &scData.scene->AllModels[x]->Meshes[y])
						{
							node_flags |= ImGuiTreeNodeFlags_Selected;
						}

						bool open = ImGui::TreeNodeEx((void*)(intptr_t)y, node_flags, scData.scene->AllModels[x]->Meshes[y].Name.c_str());
						if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
							scData.selected = &scData.scene->AllModels[x]->Meshes[y];


						if (open) {
							TransformComponent& t = *scData.scene->AllModels[x]->Meshes[y].GetComponent<TransformComponent>();

							ImGui::DragFloat3("Position", glm::value_ptr(t.Position));
							ImGui::DragFloat3("Rotation", glm::value_ptr(t.Rotation), 1, 0, 360);
							ImGui::DragFloat3("Scaling", glm::value_ptr(t.Scaling));

							ImGui::TreePop();
						}
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
				scData.scene->AddPointLight(glm::vec3(0.0), glm::vec3(1.0));
			}

			for (int x = 0; x < scData.scene->PointLights.size(); x++)
			{
				ImGuiTreeNodeFlags node_flags = entity_base_flags;
				if (scData.selected == scData.scene->PointLights[x])
				{
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}

				bool open = ImGui::TreeNodeEx((void*)(intptr_t)x, node_flags, scData.scene->PointLights[x]->Name.c_str());
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					scData.selected = scData.scene->PointLights[x];

				if (open)
				{
					TransformComponent& t = *scData.scene->PointLights[x]->GetComponent<TransformComponent>();

					ImGui::DragFloat3("Position", glm::value_ptr(t.Position));
					ImGui::DragFloat3("Rotation", glm::value_ptr(t.Rotation));
					ImGui::DragFloat3("Scaling", glm::value_ptr(t.Scaling));

					scData.scene->PointLights[x]->SetUniformPosition(t.Position);
					// scData.scene->PointLights[x]->SetColor(color);

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		RenameFunction();

		ImGui::End();
	}
}
