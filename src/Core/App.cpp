#include "App.h"

namespace Cheets
{
	App::App()
	{
		
	}

	App::~App()
	{
		
	}

}


Cheets::Application *Cheets::createApplication()
{
	return new Cheets::App();
}
