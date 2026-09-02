#include "App.h"
#include "Bron/Core/EntryPoint.h"

namespace Bron::Editor
{
	App::App()
		: editorLayer(&appLayer)
	{
		BR_PROFILE_FUNCTION();
		
		overlay.insertLayer(&appLayer);
		overlay.insertLayer(&editorLayer);
		addOverlay(&overlay);
	}

	App::~App()
	{
		
	}

}

Bron::Application* Bron::createApplication()
{
	return new Bron::Editor::App();
}
