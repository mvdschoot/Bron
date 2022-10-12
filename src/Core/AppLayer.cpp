#include "AppLayer.h"

namespace Cheets
{
	void AppLayer::OnAttach() {
		m_Width = Application::getWindow()->getWindowWidth();
		m_Height = Application::getWindow()->getWindowHeight();

		m_RickEntity = m_Registry.CreateEntity();
		m_BlockEntity = m_Registry.CreateEntity();

		m_Registry.GroupComponents<TransformComponent, SizeComponent>();
		m_Registry.AddComponent(m_RickEntity, TransformComponent({ -0.5, -0.5, 0 }));
		m_Registry.AddComponent(m_BlockEntity, TransformComponent({ 0.5, 0.5, 0 }));
		m_Registry.AddComponent(m_RickEntity, SizeComponent({ 0.5, 0.5, 0 }));
		m_Registry.AddComponent(m_BlockEntity, SizeComponent({ 0.5, 0.5, 0 }));
		m_Registry.DestroyComponent<SizeComponent>(m_BlockEntity);
		m_Registry.AddComponent(m_BlockEntity, SizeComponent({ 0.5, 0.5, 0 }));

		SpriteComponent sprite = SpriteComponent("../../../Assets/muscular_rick.png");
		m_Registry.AddComponent(m_RickEntity, std::move(sprite));
		ComponentHandle<SpriteComponent>* a = m_Registry.GetComponentE<SpriteComponent>(m_RickEntity);
		m_Registry.AddComponent(m_BlockEntity, ColorComponent(0.9f, 0.1f, 0.5f, 1.0f));

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

		Renderer2D::BeginScene(*m_Camera);
		TransformComponent& trans = GET_COMPONENTE(TransformComponent, m_RickEntity);
		Renderer2D::DrawQuad(trans, GET_COMPONENTE(SizeComponent, m_RickEntity), GET_COMPONENTE(SpriteComponent, m_RickEntity));
		Renderer2D::DrawQuad(GET_COMPONENTE(TransformComponent, m_BlockEntity),GET_COMPONENTE(SizeComponent, m_BlockEntity),GET_COMPONENTE(ColorComponent, m_BlockEntity));
		Renderer2D::EndScene();

		m_Framebuffer->unbind();
	}

	void AppLayer::OnImGuiRender() {
		ImGui::Begin("test");
		ImGui::Text("FPS: %f", 1.0f/m_Ts.getSeconds());
		ImGui::End();
	}
}
