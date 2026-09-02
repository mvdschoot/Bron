#include "AppLayer.h"

#include "ImGuizmo.h"
#include "Panels/SceneHierarchy.h"
#include "Bron/Util/Paths.h"

namespace Bron::Editor
{
	void AppLayer::OnAttach() {
		// R2D::Init();
		Command::Init();
		SceneRenderer::Init();
		// TextRenderer::Init();
		// TextRenderer::LoadUnicodeFont("../../../Assets/bitter/BitterPro-Regular.ttf", 200);

		Width = Application::getWindow()->getWindowWidth();
		Height = Application::getWindow()->getWindowHeight();
		
		FrSpec.width = Width;
		FrSpec.height = Height;
		mFramebuffer = Framebuffer::Create(FrSpec);
		mFramebuffer->unbind(); 
		
		Camera = new FrustumCamera(glm::radians(80.0F), (float)Width / (float)Height, 0.1f, 100.0f, Pos, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f });
		Sc.camera = Camera;

		Sc.CreatePhongModel("The model", Paths::ProjectAssetString("mymodel/untitled.glb").c_str());


		Sc.CreatePointLight({ 4.0, 2.0, 4.0 }, { 1.0, 1.0, 1.0 });
		Sc.CreatePointLight({ -2.0, 2.0, -2.0 }, { 1.0, 1.0, 1.0 });

		Command::ClearColor({0.0, 0.0, 0.0, 0.5});

		GridRenderer::Init(Camera);
	}
	
	void AppLayer::OnDetach() {

	}

	void AppLayer::OnEvent(Bron::Event &event)
	{
		Bron::EventDispatcher e(event);
		e.Dispatch<Bron::MouseScrolledEvent>(BIND_EVENT_FN(AppLayer::OnMouseScrolled));
	}

	bool AppLayer::OnMouseScrolled(Bron::MouseScrolledEvent& e)
	{
		Radius -= e.getOffsetY() * 0.15f;
		Radius = std::max(Radius, 0.15f);
		Pos.x = cos(XZAngle) * Radius;
		Pos.z = sin(XZAngle) * Radius;

		return true;
	}

	void AppLayer::IsKeyPressed(const Timestep ts)
	{
		float dt = ts.getSeconds();
		// Horizontal rotation (XZ plane)
		if (Bron::Input::isKeyPressed(Bron::Key::A))
		{
			XZAngle += dt;
		}
		if (Bron::Input::isKeyPressed(Bron::Key::D))
		{
			XZAngle -= dt;
		}

		// Vertical rotation (Y axis tilt)
		if (Bron::Input::isKeyPressed(Bron::Key::W))
		{
			YAngle += (YAngle > 0.5f * PI ? 0 : dt);
		}
		if (Bron::Input::isKeyPressed(Bron::Key::S))
		{
			YAngle -= (YAngle < -0.5f * PI ? 0 : dt);
		}

		// Compute new position relative to Center
		glm::vec3 center = Camera->GetDirection();
		Pos.x = center.x + cos(XZAngle) * cos(YAngle) * Radius;
		Pos.z = center.z + sin(XZAngle) * cos(YAngle) * Radius;
		Pos.y = center.y + sin(YAngle) * Radius;


		if (Bron::Input::isKeyPressed(Bron::Key::T)) {
			SceneHierarchyPanel::Data.selectedObjectOperation = ImGuizmo::OPERATION::TRANSLATE;
		}

		if (Bron::Input::isKeyPressed(Bron::Key::R)) {
			SceneHierarchyPanel::Data.selectedObjectOperation = ImGuizmo::OPERATION::ROTATE;
		}

		if (Bron::Input::isKeyPressed(Bron::Key::H)) {
			SceneHierarchyPanel::Data.selectedObjectOperation = ImGuizmo::OPERATION::SCALE;
		}

		if (Bron::Input::isKeyPressed(Bron::Key::F)) {
			if (SceneHierarchyPanel::Data.selectedObject != entt::null) {
				const TransformComponent& selectedObjectTrans =
					Sc.reg.get<TransformComponent>(SceneHierarchyPanel::Data.selectedObject);

				// Set the Camera target and reset the X,Y,Z angles
				Camera->SetTarget(selectedObjectTrans.Position);
				XZAngle = 0;
				YAngle = 0.5;

				// Update to the new position.
				Pos.x = selectedObjectTrans.Position.x + cos(XZAngle) * cos(YAngle) * Radius;
				Pos.z = selectedObjectTrans.Position.z + sin(XZAngle) * cos(YAngle) * Radius;
				Pos.y = selectedObjectTrans.Position.y + sin(YAngle) * Radius;
			}
		}
	}


	void AppLayer::OnUpdate(const Timestep ts) {
		Ts = ts;

		mFramebuffer->bind();
		Command::clear();


		IsKeyPressed(ts);
		Camera->SetPosition(Pos);

		Command::EnableBlend();
		GridRenderer::Draw();

		Command::EnableDepth();
		SceneRenderer::Draw(Sc);

		mFramebuffer->unbind();
	}
	 
	void AppLayer::OnImGuiRender() {
		
	}
}
