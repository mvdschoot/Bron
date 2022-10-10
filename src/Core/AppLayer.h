#ifndef __APP_LAYER_HEADER__
#define __APP_LAYER_HEADER__

#include "Cheets.h"
#include "Miscellaneous/Components.h"

#define GET_COMPONENTE_HANDLE(type, entity) *m_Registry.GetComponentE<type>(entity)
#define GET_COMPONENTE(type, entity) **m_Registry.GetComponentE<type>(entity)

namespace Cheets
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

		Ref<Framebuffer> m_Framebuffer;

		Ref<OrthographicCamera> m_Camera;
		float m_Zoom = 1.0f;
		glm::vec3 m_Pos{0.0f, 0.0f, -1.0f};

		// Registry
		Registry m_Registry;
		UUID m_RickEntity;
		UUID m_BlockEntity;
	};
}

#endif
