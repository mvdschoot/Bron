#include "App.h"
#include "Steve/src/Steve/Core/EntryPoint.h"

namespace Steve
{
	App::App()
		: editorLayer(&appLayer)
	{
		CH_PROFILE_FUNCTION();
		
		overlay.insertLayer(&appLayer);
		overlay.insertLayer(&editorLayer);
		addOverlay(&overlay);
	}

	App::~App()
	{
		
	}

}

Steve::Application* Steve::createApplication()
{
	return new Steve::App();
}
