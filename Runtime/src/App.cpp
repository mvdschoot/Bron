#include "App.h"

#include "Bron/Core/EntryPoint.h"

namespace bron::runtime {
App::App() {
	BR_PROFILE_FUNCTION();

	PushLayer(&world_layer_);
}
} // namespace bron::runtime

bron::Application* bron::CreateApplication() { return new bron::runtime::App(); }
