#pragma once

#include "Panels/Panel.h"

namespace Bron::Editor
{
	/// Placeholder for the project/asset browser. For now it only exposes saving and loading
	/// the scene, which is what the browser will end up driving.
	class ProjectPanel final : public Panel
	{
	public:
		explicit ProjectPanel(EditorContext& context) : Panel(context) {}

		void OnImGuiRender() override;

		void SaveScene();
		void LoadScene();
	};
}
