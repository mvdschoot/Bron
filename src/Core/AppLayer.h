#ifndef __APP_LAYER_HEADER__
#define __APP_LAYER_HEADER__

#include "Steve/src/Steve.h"

namespace Steve
{
	class AppLayer final : public Layer
	{
		friend class EditorLayer;

	public:
		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& event) override;
		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;

		bool OnMouseScrolled(MouseScrolledEvent& e);
		void IsKeyPressed(Timestep ts);


	private:
		uint32_t Width = 0;
		uint32_t Height = 0;

		Ref<Framebuffer> mFramebuffer;
		FramebufferSpecification FrSpec;

		FrustumCamera* Camera;
		float XZAngle = 0;
		float YAngle = 0.5;
		float Radius = 10.0f;
		glm::vec3 Pos{cos(XZAngle) * Radius, sin(YAngle) * Radius, sin(XZAngle) * Radius};

		Scene Sc;

		Steve::Timestep Ts;
	};
}

#endif
