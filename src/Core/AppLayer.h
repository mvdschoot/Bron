#ifndef __APP_LAYER_HEADER__
#define __APP_LAYER_HEADER__

#include "Cheets/Events/MouseEvent.h"
#include "Cheets/Renderer/OrthographicCamera.h"
#include "Core.h"

namespace Cheets
{
	class AppLayer : public Layer
	{
		public:
			virtual void OnAttach() override;
			virtual void OnDetach() override;
			virtual void OnEvent(Event& event) override;
			virtual void OnUpdate(Timestep ts) override;
			virtual void OnImGuiRender() override;
			bool onMouseScrolled(MouseScrolledEvent& e);
		private:
			uint32_t m_Width;
			uint32_t m_Height;

			Ref<Framebuffer> m_Framebuffer;
	
			Ref<OrthographicCamera> m_Camera;
			float m_Zoom = 0.0f;
			glm::vec3 m_Pos{0.0f, 0.0f, -1.0f};

			Ref<Texture> m_Texture;
	};
}

#endif
