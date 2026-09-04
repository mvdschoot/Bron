#pragma once

#include "Bron/Core/Application.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"

extern bron::Application* bron::CreateApplication();

int main(int argc, char** argv) {
	bron::Logger::Init();
	bron::Application* app = bron::CreateApplication();
	app->Run();

	delete app;
}
