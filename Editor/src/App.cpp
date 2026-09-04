#include "App.h"

#include "Bron/Core/EntryPoint.h"

namespace Bron::Editor
{
	App::App()
	{
		BR_PROFILE_FUNCTION();

		overlay.insertLayer(&editorLayer);
		addOverlay(&overlay);
	}
}

Bron::Application* Bron::createApplication()
{
	return new Bron::Editor::App();
}
