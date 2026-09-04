#pragma once

#include "Bron.h"

#include "Core/EditorLayer.h"

namespace Bron::Editor
{
	/// The editor executable. It exists to stand one layer up; all of the editor lives there.
	class App final : public Application
	{
	public:
		App();

	private:
		Overlay overlay;
		EditorLayer editorLayer;
	};
}
