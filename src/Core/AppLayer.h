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

		Ref<Framebuffer> Framebuffer;
		FramebufferSpecification FrSpec;

		FrustumCamera* Camera;
		float XZAngle = PI * -0.5;
		float YAngle = 0;
		float Radius = 10.0f;
		float Zoom = 1.0f;
		glm::vec3 Pos{0.0f, 0.0f, -1.0f};

		StandardCubeComponent* Cube;

		// Registry
		Scene Sc;

		// timestep
		Steve::Timestep Ts;
	};
}

#endif
