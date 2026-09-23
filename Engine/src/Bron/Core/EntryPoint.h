#pragma once

#include "Bron/Core/Application.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"

extern bron::Application* bron::CreateApplication(bron::CommandLineArgs args);

int main(int argc, char** argv) {
	bron::Logger::Init();
	bron::Application* app = bron::CreateApplication({.count = argc, .values = argv});
	app->Run();

	delete app;
}
