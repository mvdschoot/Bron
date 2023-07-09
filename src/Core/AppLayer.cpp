#include "AppLayer.h"

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
		m_Scene.CreateStandardModel("C:/Users/mathi/Documents/LuigiMaker/Assets/mymodel/untitled.fbx");
		// m_Scene.CreateStandardModel("C:/Users/mathi/Documents/LuigiMaker/Assets/big_car/textures/911_scene.obj");

		m_Scene.AddPointLight({ 2.0, 2.0, 2.0 }, {1.0, 1.0, 0.0}); 

		Command::ClearColor({1.0, 0.0, 1.0, 0.5});

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

		// R2D::BeginScene(m_Camera.get());
		// R2D::DrawQuad(m_Scene.GetComponentE<TransformComponent>(m_RickEntity), m_Scene.GetComponentE<SizeComponent>(m_RickEntity), m_Scene.GetComponentE<SpriteComponent>(m_RickEntity));
		// R2D::DrawQuad(m_Scene.GetComponentE<TransformComponent>(m_BlockEntity), m_Scene.GetComponentE<SizeComponent>(m_BlockEntity), m_Scene.GetComponentE<ColorComponent>(m_BlockEntity));
		// for (int x = 0; x < 16000; x++) {
		// 	R2D::DrawQuad(glm::vec2(1.0f), glm::vec2(1.0f), glm::vec4(1.0f));
		// }
		// R2D::EndScene();

		// Ref<OrthographicCamera> c = createRef<OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f,
		// 	(float)m_Width / (float)m_Height, glm::vec3{ 0.0f, 0.0f, -1.0f }, 0.0f);
		// R2D::BeginScene(c.get());
		// TextRenderer::RenderText("gefopt kanker sukkel", 0, 0, 1e-3, { 0.0,0.0,0.0,0.0 });
		// R2D::EndScene();

		// PointLight light;
		// light.Position = { 2.0, 2.0, 2.0 };
		// light.Ambient = glm::vec3(1.0f);
		// light.Diffuse = glm::vec3(1.0f);
		// light.Specular = glm::vec3(1.0f);
		// light.show = true;


		graphics::SceneRenderer::Draw(m_Scene);

	}
	 
	void AppLayer::OnImGuiRender() {
		ImGui::Begin("test");

		ImGui::Text("FPS: %f", 1000.0f / m_Ts.getMilliseconds());
		
		ImGui::DragFloat3("Light position", m_LightPos, 0.1);
		ImGui::DragFloat3("Light color", m_LightColor, 0.1);
		m_Scene.PointLights[0]->changePosition(*(glm::vec3*)m_LightPos);
		m_Scene.PointLights[0]->changeColor(*(glm::vec3*)m_LightColor);

		StandardMaterialBuffer* mat = (StandardMaterialBuffer*)m_Scene.AllModels[0]->Meshes[0].pContext->pMaterial->Data;
		ImGui::DragFloat("Shininess first", (float*)&mat->Shininess);


		ImGui::End();
	}
}
