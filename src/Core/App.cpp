#include "App.h"

namespace Steve
{
	App::App()
	{
		CH_PROFILE_FUNCTION();
		
		m_overlay.insertLayer(&m_applayer);
		addOverlay(&m_overlay);
	}

	App::~App()
	{
		
	}

}

Steve::Application *Steve::createApplication()
{
	return new Steve::App();
}
