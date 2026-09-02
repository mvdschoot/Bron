#pragma once

#include "Bron.h"

#include "AppLayer.h"
#include "EditorLayer.h"

namespace Bron::Editor
{
	class App : public Application
	{
	public:
		App();
		~App();

	private:
		Overlay overlay;
		AppLayer appLayer;
		EditorLayer editorLayer;
	};
}
