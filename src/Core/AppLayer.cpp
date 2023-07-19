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

		Width = Application::getWindow()->getWindowWidth();
		Height = Application::getWindow()->getWindowHeight();
		
		FrSpec.width = Width;
		FrSpec.height = Height;
		Framebuffer = Framebuffer::Create(FrSpec);
		Framebuffer->unbind();
		
		Camera = new FrustumCamera(glm::radians(80.0F), (float)Width / (float)Height, 0.1f, 100.0f, Pos, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f });
		Sc.Camera = Camera;
		Sc.CreateStandardModel("The model", "C:/Users/mathi/Documents/Steve/Assets/mymodel/untitled.obj");

		// Sc.CreateStandardModel("C:/Users/mathi/Documents/Steve/Assets/big_car/textures/911_scene.obj");

		Sc.AddPointLight({ 4.0, 2.0, 4.0 }, { 1.0, 1.0, 1.0 });

		LightData& l = *Sc.PointLights[0]->GetComponent<LightData>();
		Sc.AddPointLight({ -2.0, 2.0, -2.0 }, { 1.0, 1.0, 1.0 });

		Command::ClearColor({1.0, 0.0, 1.0, 0.5});

		Cube = Sc.AddCube("The cube", {0,3,0}, {1,1,1});
		Cube->AddComponent<CubeCollisionBody>({{true, Cube->GetComponent<TransformComponent>()}, {1,1,1} });

		Bvh.AddNode(Cube);
		// Bvh.AddNode(Sc.AllModels[0]);
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
		Radius -= e.getOffsetY() * 0.15f;
		Radius = std::max(Radius, 0.15f);
		Pos.x = cos(XZAngle) * Radius;
		Pos.z = sin(XZAngle) * Radius;

		return true;
	}

	void AppLayer::IsKeyPressed(const Timestep ts)
	{
		float dt = ts.getSeconds();
		if (Steve::Input::isKeyPressed(Steve::Key::A))
		{
			XZAngle += dt;
			Pos.x = cos(XZAngle) * Radius;
			Pos.z = sin(XZAngle) * Radius;
		}
		if (Steve::Input::isKeyPressed(Steve::Key::D))
		{
			XZAngle -= dt;
			Pos.x = cos(XZAngle) * Radius;
			Pos.z = sin(XZAngle) * Radius;
		}

		if (Steve::Input::isKeyPressed(Steve::Key::W))
		{
			YAngle += YAngle > 0.5*PI ? 0 : dt;
			Pos.y = sin(YAngle) * Radius;
		}

		if (Steve::Input::isKeyPressed(Steve::Key::S))
		{
			YAngle -= YAngle < -0.5*PI ? 0 : dt;
			Pos.y = sin(YAngle) * Radius;
		}
	}


	void AppLayer::OnUpdate(const Timestep ts) {
		Ts = ts;

		// Framebuffer->bind();
		Command::clear();


		IsKeyPressed(ts);
		Camera->SetPosition(Pos);

		TransformComponent& t = Sc.PointLights[0]->GetComponent<TransformComponent>();

		graphics::SceneRenderer::Draw(Sc);

		Framebuffer->unbind();
	}
	 
	void AppLayer::OnImGuiRender() {
		
	}
}
