#include "App.h"

#include "Bron/Core/EntryPoint.h"

namespace bron::runtime {
App::App(const std::filesystem::path& root) : world_layer_(root) {
	BR_PROFILE_FUNCTION();

	PushLayer(&world_layer_);
}
} // namespace bron::runtime

bron::Application* bron::CreateApplication(const CommandLineArgs args) {
	// The one argument the runtime takes: the directory holding the game to run. Handy
	// for testing an export without copying the executable into it, and for keeping
	// several games beside one binary.
	const std::optional<std::string_view> root = args.Get(0);

	return new bron::runtime::App(root.has_value() ? std::filesystem::path(*root) : std::filesystem::path());
}
