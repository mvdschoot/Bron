#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Layers/Layer.h"
#include "Bron/Core/Window.h"
#include "Bron/Core/Profiling.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <memory>

namespace Bron
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
