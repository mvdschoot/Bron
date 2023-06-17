#include "AppLayer.h"

namespace Steve
{
	using namespace render;

	void AppLayer::OnAttach() {
		R2D::Init();
		Renderer3D::Init();
		LineRenderer::Init();
		TextRenderer::Init();
		TextRenderer::LoadUnicodeFont("../../../Assets/bitter/BitterPro-Regular.ttf", 200);

		m_Width = Application::getWindow()->getWindowWidth();
		m_Height = Application::getWindow()->getWindowHeight();

		m_RickEntity = m_Scene.CreateEntity("Rickert");
		m_BlockEntity = m_Scene.CreateEntity("Block");
		m_ModelEntity = m_Scene.CreateEntity("Model");
		m_CubeEntity = m_Scene.CreateEntity("Light");
		m_CubeMidEntity = m_Scene.CreateEntity("Mid");

		View<TransformComponent, SizeComponent> view = m_Scene.GroupComponents<TransformComponent, SizeComponent>();
		m_Scene.AddComponent(m_RickEntity, TransformComponent({ -0.5, -0.5, 0 }));
		m_Scene.AddComponent(m_BlockEntity, TransformComponent({ 0.5, 0.5, 0 }));
		m_Scene.AddComponent(m_RickEntity, SizeComponent({ 0.5, 0.5, 0 }));
		m_Scene.AddComponent(m_BlockEntity, SizeComponent({ 0.5, 0.5, 0 }));
		m_Scene.DestroyComponent<SizeComponent>(m_BlockEntity);
		m_Scene.AddComponent(m_BlockEntity, SizeComponent({ 0.5, 0.5, 0 }));

		m_Scene.AddComponent(m_RickEntity, SpriteComponent("../../../Assets/muscular_rick.png"));
		m_Scene.AddComponent(m_BlockEntity, ColorComponent(0.9f, 0.1f, 0.5f, 1.0f));
		m_Scene.AddComponent(m_CubeEntity, CubeComponent({ 2.0, 2.0, 2.0 }, { 0.2, 0.2, 0.2 }));

		// add cube & set color
		std::get<1>(m_Scene.AddComponent(m_CubeMidEntity, CubeComponent({ -0.5, -0.5, -0.5 }, { 1.0, 1.0, 1.0 }))).SetColor({ 0.1, 0.9, 0.5 });

		// add model & set uniform callback
		std::vector<MeshComponent>& meshes = std::get<1>(m_Scene.AddComponent(m_ModelEntity, ModelComponent("../../../Assets/big_car/textures/911_scene.obj"))).GetMeshes();
		for(MeshComponent& mesh : meshes)
		{
			mesh.SetUniformCallback(std::function<void(Ref<Shader>&)>(this->UniformSetter));
		}

		View<TransformComponent, SizeComponent>::Iterator it = View<TransformComponent, SizeComponent>::FilterBy([](Entity* ent)
		{
			return ent->Contains<ColorComponent>();
		});
		/*for (; it != View<TransformComponent, SizeComponent>::end(); ++it)
		{
			auto ok = *it;
			int a = 5;
		}*/

		FramebufferSpecification spec;
		spec.width = m_Width;
		spec.height = m_Height;
		m_Framebuffer = Framebuffer::Create(spec);
		
		/*m_Camera = Steve::createRef<Steve::OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f,
				(float)m_Width / (float)m_Height, m_Pos, 0.0f);
		m_Camera->SetZoom(m_Zoom);*/
		m_Camera = createRef<FrustumCamera>(glm::radians(80.0F), (float)m_Width / (float)m_Height, 0.1f, 100.0f, m_Pos, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f });

		Command::ClearColor({1.0, 0.0, 1.0, 0.5});

	}
	
	void AppLayer::OnDetach() {
		Renderer3D::ShutDown();
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

	void AppLayer::UniformSetter(Ref<Shader>& shader)
	{
		shader->set
	}

	void AppLayer::OnUpdate(const Timestep ts) {
		m_Ts = ts;

		//m_Framebuffer->bind();
		Command::clear();


		IsKeyPressed(ts);
		m_Camera->SetPosition(m_Pos);

		// Renderer2D::BeginScene(m_Camera.get());
		// Renderer2D::DrawQuad(m_Scene.GetComponentE<TransformComponent>(m_RickEntity), m_Scene.GetComponentE<SizeComponent>(m_RickEntity), m_Scene.GetComponentE<SpriteComponent>(m_RickEntity));
		// Renderer2D::DrawQuad(m_Scene.GetComponentE<TransformComponent>(m_BlockEntity), m_Scene.GetComponentE<SizeComponent>(m_BlockEntity), m_Scene.GetComponentE<ColorComponent>(m_BlockEntity));
		// for (int x = 0; x < 16000; x++) {
		// 	Renderer2D::DrawQuad(glm::vec2(1.0f), glm::vec2(1.0f), glm::vec4(1.0f));
		// }
		// Renderer2D::EndScene();

		// Ref<OrthographicCamera> c = createRef<OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f,
		// 	(float)m_Width / (float)m_Height, glm::vec3{ 0.0f, 0.0f, -1.0f }, 0.0f);
		// Renderer2D::BeginScene(c.get());
		// TextRenderer::RenderText("gefopt kanker sukkel", 0, 0, 1e-3, { 0.0,0.0,0.0,0.0 });
		// Renderer2D::EndScene();

		PointLight light;
		light.Position = { 2.0, 2.0, 2.0 };
		light.Ambient = glm::vec3(1.0f);
		light.Diffuse = glm::vec3(1.0f);
		light.Specular = glm::vec3(1.0f);
		light.show = true;

		Renderer3D::BeginScene(m_Camera.get());
		Renderer3D::AddLight(light);
		ModelComponent& model = m_Scene.GetComponentE<ModelComponent>(m_ModelEntity);
		for(MeshComponent& mesh : model.GetMeshes())
		{
			Renderer3D::DrawModel(&mesh);
		}
		Renderer3D::DrawModel(&m_Scene.GetComponentE<CubeComponent>(m_CubeEntity).GetMesh());
		Renderer3D::DrawModel(&m_Scene.GetComponentE<CubeComponent>(m_CubeMidEntity).GetMesh());
		Renderer3D::EndScene();


		m_Framebuffer->unbind();
	}
	 
	void AppLayer::OnImGuiRender() {
		ImGui::Begin("test");
		
		ImGui::DragFloat("Specular exponent", &m_Specular, 0.0, 1.0, 50.0f);


		auto& meshes = m_Scene.GetComponentE<ModelComponent>(m_ModelEntity).GetMeshes();
		for(MeshComponent& mesh : meshes)
		{
			Ref<MaterialComponent>& material = mesh.GetMaterial();
		}

		ImGui::Text("FPS: %f\n", 1.0f / m_Ts.getSeconds());
		ImGui::Text("QuadCount: %u", R2D::GetTotQuadCount());
		ImGui::Text("QuadIndexCount: %u\n\n", R2D::GetTotQuadIndexCount());
		ImGui::Text("3D index count: %u", Renderer3D::GetIndexCount());
		ImGui::Text("QuadIndexCount: %u\n\n", R2D::GetTotQuadIndexCount());
		ImGui::Text("LineCount: %u", LineRenderer::GetLineCount());
		ImGui::Text("LineIndexCount: %u\n", LineRenderer::GetLineIndexCount());
		ImGui::Text("Strip: LineCount: %u", LineRenderer::GetLineStripCount());
		ImGui::Text("Strip: LineIndexCount: %u\n", LineRenderer::GetLineStripIndexCount());
		ImGui::End();
	}
}
