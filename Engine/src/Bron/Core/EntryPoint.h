#pragma once

#include "Bron/Core/Application.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"

extern Bron::Application* Bron::createApplication();

int main(int argc, char** argv)
{
	Bron::Logger::Init();
	Bron::Application* app = Bron::createApplication();
	app->Run();

	delete app;
}
