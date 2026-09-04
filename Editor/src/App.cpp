#include "App.h"

#include "Bron/Core/EntryPoint.h"

#include "Core/Preferences.h"

namespace bron::editor
{
	App::App()
	{
		BR_PROFILE_FUNCTION();

		overlay.InsertLayer(&editor_layer);
		AddOverlay(&overlay);
	}

	App::~App()
	{
		// Application never calls Layer::OnDetach, so shutdown work hangs off the app itself.
		Preferences::Save();
	}
}

bron::Application* bron::CreateApplication()
{
	// Before the Application constructor, which is what creates the window: the theme and
	// the UI scale have to be known by the time anything is drawn.
	bron::editor::Preferences::Load();

	return new bron::editor::App();
}
