#include "AppLayer.h"
#include "Cheets/Core/Core.h"
#include "Cheets/Renderer/Framebuffer.h"
#include "Cheets/Renderer/OrthographicCamera.h"
#include "Cheets/Renderer/Renderer2D.h"

namespace Cheets
{
		
	void AppLayer::OnAttach() {
		m_Width = Application::getWindow()->getWindowWidth();
		m_Height = Application::getWindow()->getWindowHeight();

		FramebufferSpecification spec;
		spec.width = m_Width;
		spec.height = m_Height;
		m_Framebuffer = Framebuffer::Create(spec);

		m_Texture = Texture2D::Create("../../src/Assets/muscular_rick.png");
		m_Camera = Cheets::createRef<Cheets::OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f,
				(float)m_Width / (float)m_Height, m_Pos, 0.0f);

		RendererCommand::ClearColor({1.0, 0.0, 1.0, 0.5});
	}
	
	void AppLayer::OnDetach() {

	}

	void AppLayer::OnEvent(Cheets::Event &event)
	{
		// APP_INFO("Event: {}", event.GetName());
		Cheets::EventDispatcher e(event);
		e.Dispatch<Cheets::MouseScrolledEvent>(BIND_EVENT_FN(AppLayer::onMouseScrolled));
	}

	bool AppLayer::onMouseScrolled(Cheets::MouseScrolledEvent& e)
	{
		m_Zoom -= e.getOffsetY() * 0.15f;
		m_Zoom = std::max(m_Zoom, 0.15f);

		m_Camera->setZoom(m_Zoom);
		return true;
	}

	void AppLayer::OnUpdate(Timestep ts) {
		m_Framebuffer->bind();
		RendererCommand::clear();

		Renderer2D::beginScene(*m_Camera);
		Renderer2D::drawQuad({-0.25, -0.25}, {0.5, 0.5}, m_Texture);
		Renderer2D::endScene();

		m_Framebuffer->unbind();
	}

	void AppLayer::OnImGuiRender() {
		ImGui::Begin("test");

		uint32_t textureID = m_Framebuffer->getColorAttachID();
		ImGui::Image((void*)(intptr_t)textureID, ImVec2(355, 200), {0, 1}, {1, 0});
		
		ImGui::End();
	}
}
