#ifndef IMGUILAYER_HEADER
#define IMGUILAYER_HEADER

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Layers/Layer.h"
#include "Steve/Core/Window.h"
#include "Steve/Core/Profiling.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <memory>

namespace Steve
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer(Ref<Window> window);

		~ImGuiLayer() override
		{
		}

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& event) override;
		void OnUpdate(Timestep ts) override;

		void OnImGuiRender() override
		{
		};

		void begin();
		void end();

	private:
		Ref<Window> _window;
	};
}


#endif
