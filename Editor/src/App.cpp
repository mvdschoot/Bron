#include "App.h"

#include "Bron/Core/EntryPoint.h"

#include "Core/Preferences.h"

namespace Bron::Editor
{
	App::App()
	{
		BR_PROFILE_FUNCTION();

		overlay.insertLayer(&editorLayer);
		addOverlay(&overlay);
	}

	App::~App()
	{
		// Application never calls Layer::OnDetach, so shutdown work hangs off the app itself.
		Preferences::Save();
	}
}

Bron::Application* Bron::createApplication()
{
	// Before the Application constructor, which is what creates the window: the theme and
	// the UI scale have to be known by the time anything is drawn.
	Bron::Editor::Preferences::Load();

	return new Bron::Editor::App();
}
