#include "AppLayer.h"

namespace Cheets
{
	void AppLayer::OnAttach() {
		Renderer2D::Init();
		LineRenderer::Init();
		TextRenderer::Init();
		TextRenderer::LoadUnicodeFont("../../../Assets/bitter/BitterPro-Regular.ttf", 200);

		m_Width = Application::getWindow()->getWindowWidth();
		m_Height = Application::getWindow()->getWindowHeight();

		m_RickEntity = m_Scene.CreateEntity("Rickert");
		m_BlockEntity = m_Scene.CreateEntity("Block");

		View<TransformComponent, SizeComponent> view = m_Scene.GroupComponents<TransformComponent, SizeComponent>();
		m_Scene.AddComponent(m_RickEntity, TransformComponent({ -0.5, -0.5, 0 }));
		m_Scene.AddComponent(m_BlockEntity, TransformComponent({ 0.5, 0.5, 0 }));
		m_Scene.AddComponent(m_RickEntity, SizeComponent({ 0.5, 0.5, 0 }));
		m_Scene.AddComponent(m_BlockEntity, SizeComponent({ 0.5, 0.5, 0 }));
		m_Scene.DestroyComponent<SizeComponent>(m_BlockEntity);
		m_Scene.AddComponent(m_BlockEntity, SizeComponent({ 0.5, 0.5, 0 }));

		m_Scene.AddComponent(m_RickEntity, SpriteComponent("../../../Assets/muscular_rick.png"));
		m_Scene.AddComponent(m_BlockEntity, ColorComponent(0.9f, 0.1f, 0.5f, 1.0f));

		View<TransformComponent, SizeComponent>::Iterator it = View<TransformComponent, SizeComponent>::FilterBy([](Entity* ent)
		{
			return ent->Contains<ColorComponent>();
		});
		for (; it != View<TransformComponent, SizeComponent>::end(); ++it)
		{
			auto ok = *it;
			int a = 5;
		}

		FramebufferSpecification spec;
		spec.width = m_Width;
		spec.height = m_Height;
		m_Framebuffer = Framebuffer::Create(spec);
		
		m_Camera = Cheets::createRef<Cheets::OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f,
				(float)m_Width / (float)m_Height, m_Pos, 0.0f);
		m_Camera->SetZoom(m_Zoom);

		RendererCommand::ClearColor({1.0, 0.0, 1.0, 0.5});

	}
	
	void AppLayer::OnDetach() {

	}

	void AppLayer::OnEvent(Cheets::Event &event)
	{
		// APP_INFO("Event: {}", event.GetName());
		Cheets::EventDispatcher e(event);
		e.Dispatch<Cheets::MouseScrolledEvent>(BIND_EVENT_FN(AppLayer::OnMouseScrolled));
	}

	bool AppLayer::OnMouseScrolled(Cheets::MouseScrolledEvent& e)
	{
		m_Zoom -= e.getOffsetY() * 0.15f;
		m_Zoom = std::max(m_Zoom, 0.15f);

		m_Camera->SetZoom(m_Zoom);
		return true;
	}

	void AppLayer::IsKeyPressed(const Timestep ts)
	{
		float dt = ts.getSeconds();
		if (Cheets::Input::isKeyPressed(Cheets::Key::A))
		{
			m_Pos.x -= dt;
		}
		if (Cheets::Input::isKeyPressed(Cheets::Key::D))
		{
			m_Pos.x += dt;
		}

		if (Cheets::Input::isKeyPressed(Cheets::Key::W))
		{
			m_Pos.y += dt;
		}

		if (Cheets::Input::isKeyPressed(Cheets::Key::S))
		{
			m_Pos.y -= dt;
		}
	}

	void AppLayer::OnUpdate(const Timestep ts) {
		m_Ts = ts;

		//m_Framebuffer->bind();
		RendererCommand::clear();


		IsKeyPressed(ts);
		m_Camera->SetPosition(m_Pos);

		Renderer2D::BeginScene(m_Camera.get());
		Renderer2D::DrawQuad(m_Scene.GetComponentE<TransformComponent>(m_RickEntity), m_Scene.GetComponentE<SizeComponent>(m_RickEntity), m_Scene.GetComponentE<SpriteComponent>(m_RickEntity));
		Renderer2D::DrawQuad(m_Scene.GetComponentE<TransformComponent>(m_BlockEntity), m_Scene.GetComponentE<SizeComponent>(m_BlockEntity), m_Scene.GetComponentE<ColorComponent>(m_BlockEntity));
		TextRenderer::RenderText("gefopt kanker sukkel", 0, 0, 1e-3, { 0.0,0.0,0.0,0.0 });
		Renderer2D::EndScene();

		

		LineRenderer::Start(m_Camera.get());

		int size = 50;
		int halfsize = size / 2;
		float gray_color = 200.0f;
		float line_width = 0.005;
		for(int x = -halfsize; x < halfsize; x++)
		{
			LineRenderer::DrawLine({ -halfsize, x, 0.0f }, { halfsize, x, 0.0f }, { gray_color, gray_color, gray_color, 0.8 }, line_width * m_Camera->AspectRatio);
			LineRenderer::DrawLine({ x, -halfsize, 0.0f }, { x, halfsize, 0.0f }, { gray_color, gray_color, gray_color, 0.8 }, line_width);
		}

		LineRenderer::End();

		m_Framebuffer->unbind();
	}
	 
	void AppLayer::OnImGuiRender() {
		ImGui::Begin("test");
		ImGui::Text("FPS: %f\n", 1.0f / m_Ts.getSeconds());
		ImGui::Text("QuadCount: %u", Renderer2D::GetTotQuadCount());
		ImGui::Text("QuadIndexCount: %u\n\n", Renderer2D::GetTotQuadIndexCount());
		ImGui::Text("LineCount: %u", LineRenderer::GetLineCount());
		ImGui::Text("LineIndexCount: %u\n", LineRenderer::GetLineIndexCount());
		ImGui::Text("Strip: LineCount: %u", LineRenderer::GetLineStripCount());
		ImGui::Text("Strip: LineIndexCount: %u\n", LineRenderer::GetLineStripIndexCount());
		ImGui::End();
	}
}
