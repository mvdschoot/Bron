#include "Panels/ProjectPanel.h"

#include "Bron/Scene/Serialization/Serialization.h"
#include "Bron/Util/Paths.h"

namespace Bron::Editor
{
	namespace
	{
		// Until projects carry their own scene list, everything goes to one well known file.
		const char* SceneFile = "saveFile.json";
	}

	void ProjectPanel::OnImGuiRender()
	{
		ImGui::Begin("Project explorer");

		if (ImGui::Button("Save scene"))
			SaveScene();

		if (ImGui::Button("Load scene"))
			LoadScene();

		ImGui::End();
	}

	void ProjectPanel::SaveScene()
	{
		Serialization::SerializeScene(Context.scene, Paths::ProjectAsset(SceneFile));
	}

	void ProjectPanel::LoadScene()
	{
		// The load replaces every entity, so nothing may still be holding a handle into the old scene.
		Context.ClearSelection();

		Serialization::DeserializeScene(Context.scene, Paths::ProjectAsset(SceneFile));
	}
}
