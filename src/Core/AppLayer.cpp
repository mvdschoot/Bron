#include "AppLayer.h"

#include "ImGuizmo.h"

namespace Steve
{
	using namespace graphics;

	void AppLayer::OnAttach() {
		// R2D::Init();
		SceneRenderer::Init();
		// LineRenderer::Init();
		// TextRenderer::Init();
		// TextRenderer::LoadUnicodeFont("../../../Assets/bitter/BitterPro-Regular.ttf", 200);

		m_Width = Application::getWindow()->getWindowWidth();
		m_Height = Application::getWindow()->getWindowHeight();

		FramebufferSpecification spec;
		spec.width = m_Width;
		spec.height = m_Height;
		m_Framebuffer = Framebuffer::Create(spec);
		m_Framebuffer->unbind();
		
		m_Camera = new FrustumCamera(glm::radians(80.0F), (float)m_Width / (float)m_Height, 0.1f, 100.0f, m_Pos, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f });
		m_Scene.Camera = m_Camera;
		m_Scene.CreateStandardModel("The model", "C:/Users/mathi/Documents/Steve/Assets/mymodel/untitled.obj");

		// m_Scene.CreateStandardModel("C:/Users/mathi/Documents/Steve/Assets/big_car/textures/911_scene.obj");

		m_Scene.AddPointLight({ 4.0, 2.0, 4.0 }, { 1.0, 1.0, 1.0 });
		m_Scene.AddPointLight({ -2.0, 2.0, -2.0 }, { 1.0, 1.0, 1.0 });

		Command::ClearColor({1.0, 0.0, 1.0, 0.5});

		m_Cube = m_Scene.AddCube("The cube", {0,3,0}, {1,1,1});
		m_Cube->AddComponent<CubeCollisionBody>({{true, m_Cube->GetComponent<TransformComponent>()}, {1,1,1} });

		m_BVH.AddNode(m_Cube);
		// m_BVH.AddNode(m_Scene.AllModels[0]);
	}
	
	void AppLayer::OnDetach() {

	}

	void AppLayer::OnEvent(Steve::Event &event)
	{
		// APP_INFO("Event: {}", event.GetName());
		Steve::EventDispatcher e(event);
		e.Dispatch<Steve::MouseScrolledEvent>(BIND_EVENT_FN(AppLayer::OnMouseScrolled));
	}

	bool AppLayer::OnMouseScrolled(Steve::MouseScrolledEvent& e)
	{
		m_Radius -= e.getOffsetY() * 0.15f;
		m_Radius = std::max(m_Radius, 0.15f);
		m_Pos.x = cos(m_XZAngle) * m_Radius;
		m_Pos.z = sin(m_XZAngle) * m_Radius;

		return true;
	}

	void AppLayer::IsKeyPressed(const Timestep ts)
	{
		float dt = ts.getSeconds();
		if (Steve::Input::isKeyPressed(Steve::Key::A))
		{
			m_XZAngle += dt;
			m_Pos.x = cos(m_XZAngle) * m_Radius;
			m_Pos.z = sin(m_XZAngle) * m_Radius;
		}
		if (Steve::Input::isKeyPressed(Steve::Key::D))
		{
			m_XZAngle -= dt;
			m_Pos.x = cos(m_XZAngle) * m_Radius;
			m_Pos.z = sin(m_XZAngle) * m_Radius;
		}

		if (Steve::Input::isKeyPressed(Steve::Key::W))
		{
			m_YAngle += m_YAngle > 0.5*PI ? 0 : dt;
			m_Pos.y = sin(m_YAngle) * m_Radius;
		}

		if (Steve::Input::isKeyPressed(Steve::Key::S))
		{
			m_YAngle -= m_YAngle < -0.5*PI ? 0 : dt;
			m_Pos.y = sin(m_YAngle) * m_Radius;
		}
	}


	void AppLayer::OnUpdate(const Timestep ts) {
		m_Ts = ts;

		//m_Framebuffer->bind();
		Command::clear();


		IsKeyPressed(ts);
		m_Camera->SetPosition(m_Pos);

		graphics::SceneRenderer::Draw(m_Scene);

	}
	 
	void AppLayer::OnImGuiRender() {
		ImGui::Begin("Debug");
		ImGui::Text("FPS: %f", 1000.0f / m_Ts.getMilliseconds());
		ImGui::End();

		ImGui::Begin("Scene Hierarchy");

		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		if(ImGui::TreeNode("Models"))
		{
			for(int x = 0; x < m_Scene.AllModels.size(); x++)
			{
				ImGuiTreeNodeFlags node_flags = base_flags;
				if (m_Scene.SelectedEntity == m_Scene.AllModels[x])
				{
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}

				bool open = ImGui::TreeNodeEx((void*)(intptr_t)x, node_flags, m_Scene.AllModels[x]->Name.c_str());
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					m_Scene.SelectedEntity = m_Scene.AllModels[x];

				std::string name = "Rename '" + m_Scene.AllModels[x]->Name + "'";
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
					bool text = ImGui::InputText("Name", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AlwaysInsertMode);

					if (text || ImGui::Button("OK", ImVec2(120, 0)))
					{
						m_Scene.SelectedEntity->Name = std::string(buf);
						ImGui::CloseCurrentPopup();
					}
					ImGui::SetItemDefaultFocus();
					ImGui::SameLine();
					if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }

					ImGui::EndPopup();
				}

				if (open)
				{
					if (ImGui::CollapsingHeader("Properties"))
					{
						TransformComponent& t = *m_Scene.AllModels[x]->GetComponent<TransformComponent>();

						auto [tr, ro, sc] = t.Get();

						ImGui::DragFloat3("Position", glm::value_ptr(tr));
						ImGui::DragFloat3("Rotation", glm::value_ptr(ro));
						ImGui::DragFloat3("Scaling", glm::value_ptr(sc));

						t.Set(SET_ALL3, { tr, ro, sc });
					}

					for(int y = 0; y < m_Scene.AllModels[x]->Meshes.size(); y++)
					{
						if (ImGui::TreeNode((void*)(intptr_t)y, "Mesh %d", y)) {
							TransformComponent& t = *m_Scene.AllModels[x]->Meshes[y].GetComponent<TransformComponent>();

							auto [tr, ro, sc] = t.Get();

							ImGui::DragFloat3("Position", glm::value_ptr(tr));
							ImGui::DragFloat3("Rotation", glm::value_ptr(ro));
							ImGui::DragFloat3("Scaling", glm::value_ptr(sc));

							t.Set(SET_ALL3, { tr, ro, sc });

							ImGui::TreePop();
						}
					}

					ImGui::TreePop();
				}
			}
			ImGuiTreeNodeFlags node_flags = base_flags;

			ImGui::TreePop();
		}

		if(ImGui::TreeNode("Point lights"))
		{
			if (ImGui::Button("Add pointlight")) {
				m_Scene.AddPointLight(glm::vec3(0.0), glm::vec3(1.0));
			}

			for (int x = 0; x < m_Scene.PointLights.size(); x++)
			{
				if (ImGui::TreeNode(m_Scene.PointLights[x]->Name.c_str()))
				{
					TransformComponent& t = *m_Scene.PointLights[x]->GetComponent<TransformComponent>();

					auto [tr, ro, sc] = t.Get();
					
					ImGui::DragFloat3("Position", glm::value_ptr(tr));
					ImGui::DragFloat3("Rotation", glm::value_ptr(ro));
					ImGui::DragFloat3("Scaling", glm::value_ptr(sc));

					t.Set(SET_ALL3, { tr, ro, sc });

					m_Scene.PointLights[x]->SetUniformPosition(tr);
					// m_Scene.PointLights[x]->SetColor(color);

					ImGui::TreePop(); 
				}
			}

			ImGui::TreePop();
		}

		// TransformComponent& t = *m_Cube->GetComponent<TransformComponent>();
		// ImGui::DragFloat3("Position cube", (float*) & t.Position);

		ImGui::End();
	}
}
