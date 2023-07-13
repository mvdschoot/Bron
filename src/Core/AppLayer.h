#ifndef __APP_LAYER_HEADER__
#define __APP_LAYER_HEADER__

#include "Steve/src/Steve.h"

#define PI 3.14159265358979323846

namespace Steve
{
	class AppLayer final : public Layer
	{
	public:
		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& event) override;
		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;

		bool OnMouseScrolled(MouseScrolledEvent& e);
		void IsKeyPressed(Timestep ts);


	private:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		Ref<graphics::Framebuffer> m_Framebuffer;

		graphics::FrustumCamera* m_Camera;
		float m_XZAngle = PI * -0.5;
		float m_YAngle = 0;
		float m_Radius = 10.0f;
		float m_Zoom = 1.0f;
		glm::vec3 m_Pos{0.0f, 0.0f, -1.0f};

		// Registry
		graphics::Scene m_Scene;
		UUID m_RickEntity;
		UUID m_BlockEntity;
		UUID m_ModelEntity;
		UUID m_CubeEntity;
		UUID m_CubeMidEntity;

		// timestep
		Steve::Timestep m_Ts;
	};
}

#endif
