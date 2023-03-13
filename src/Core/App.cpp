#include "App.h"

namespace Cheets
{
	App::App()
	{
		CH_PROFILE_FUNCTION();
		render::Renderer::Init();
		
		m_overlay.insertLayer(&m_applayer);
		addOverlay(&m_overlay);
	}

	App::~App()
	{
		
	}

}

Cheets::Application *Cheets::createApplication()
{
	return new Cheets::App();
}
