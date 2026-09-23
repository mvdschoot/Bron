#include "App.h"

#include "Bron/Core/EntryPoint.h"

#include "Core/Preferences.h"

namespace bron::editor {
App::App() {
	BR_PROFILE_FUNCTION();

	// A layer rather than an overlay: the ImGui layer has to stay on top of it, both to
	// draw the panels this fills and to get at the input first.
	PushLayer(&editor_layer);
}

App::~App() {
	// Anything the editor layer holds is released in its OnDetach, which Run() has
	// already called. This is only what belongs to the application itself.
	Preferences::Save();
}
} // namespace bron::editor

bron::Application* bron::CreateApplication(CommandLineArgs) {
	// Before the Application constructor, which is what creates the window: the theme and
	// the UI scale have to be known by the time anything is drawn.
	bron::editor::Preferences::Load();

	return new bron::editor::App();
}
