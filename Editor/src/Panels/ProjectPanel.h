#pragma once

#include "Panels/Panel.h"

namespace Bron::Editor
{
	/// Shows the open project and lets the scene be written and read. The asset browser will
	/// grow here; for now it is the project's settings plus the scene it starts with.
	class ProjectPanel final : public Panel
	{
	public:
		explicit ProjectPanel(EditorContext& context) : Panel(context) {}

		void OnImGuiRender() override;

		/// Writes the scene to the project's startup scene.
		void SaveScene();

		/// Replaces the scene with the project's startup scene. The project guarantees the
		/// file is there, so this has no "nothing to open" case.
		void LoadScene();

	private:
		void DrawSettings();
	};
}
