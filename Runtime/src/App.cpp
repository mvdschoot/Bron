#include "App.h"

#include "Bron/Core/EntryPoint.h"
#include "Layers/WorldLayer.h"

namespace bron::editor {
App::App() {
	BR_PROFILE_FUNCTION();

	PushLayer(WorldLayer)
}

App::~App() {}
} // namespace bron::editor

bron::Application* bron::CreateApplication() { return new bron::editor::App(); }
