#ifndef __APP_HEADER__
#define __APP_HEADER__

#include "Steve/src/Steve.h"

#include "AppLayer.h"
#include "EditorLayer.h"

namespace Steve
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

#endif
