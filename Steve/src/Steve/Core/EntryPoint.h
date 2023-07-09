#ifndef ENTRYPOINT_HEADER
#define ENTRYPOINT_HEADER

#include "Steve/Core/Application.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Core.h"
#include "Steve/Core/Profiling.h"

extern Steve::Application* Steve::createApplication();

int main(int argc, char** argv)
{
	Steve::Logger::Init();
	Steve::Application* app = Steve::createApplication();
	app->Run();

	delete app;
}

#endif
