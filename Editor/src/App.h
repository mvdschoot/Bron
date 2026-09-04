#pragma once

#include "Bron.h"

#include "Core/EditorLayer.h"

namespace bron::editor
{
	/// The editor executable. It exists to stand one layer up; all of the editor lives there.
	class App final : public Application
	{
	public:
		App();
		~App() override;

	private:
		Overlay overlay;
		EditorLayer editor_layer;
	};
}
