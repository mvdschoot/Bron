#include "Panels/ProjectPanel.h"

#include <cstdio>

#include "Bron/Core/Logger.h"
#include "Bron/Scene/Serialization/Serialization.h"

namespace bron::editor
{
	using namespace ImGui;

	void ProjectPanel::OnImGuiRender()
	{
		Begin("Project");

		if (!Context.HasProject())
		{
			// The editor starts here when nothing has been opened before.
			TextDisabled("No project open.");
			TextDisabled("Create or open one from the File menu.");
			End();
			return;
		}

		const Project& project = *Context.project;

		Text("%s", project.Settings().name.c_str());
		TextDisabled("%s", project.AssetRoot().string().c_str());

		Separator();

		DrawSettings();

		Separator();

		if (Button("Save scene"))
			SaveScene();

		SameLine();
		if (Button("Load scene"))
			LoadScene();

		End();
	}

	void ProjectPanel::DrawSettings()
	{
		if (!CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen))
			return;

		ProjectSettings& settings = Context.project->Settings();

		Indent();

		char name[128];
		std::snprintf(name, sizeof(name), "%s", settings.name.c_str());
		if (InputText("Name", name, sizeof(name)))
			settings.name = name;

		char scene[260];
		std::snprintf(scene, sizeof(scene), "%s", settings.startupScene.generic_string().c_str());
		if (InputText("Startup scene", scene, sizeof(scene)))
			settings.startupScene = scene;

		// The asset directory is not editable here: changing it invalidates every path
		// already stored in the scenes, so it belongs in a migration, not a text field.
		BeginDisabled();
		char assets[260];
		std::snprintf(assets, sizeof(assets), "%s", settings.assetDirectory.generic_string().c_str());
		InputText("Asset directory", assets, sizeof(assets));
		EndDisabled();

		if (Button("Save project"))
			Context.project->Save();

		Unindent();
	}

	void ProjectPanel::SaveScene()
	{
		Serialization::SerializeScene(Context.scene, Context.project->StartupScenePath());
	}

	void ProjectPanel::LoadScene()
	{
		// A project always has its startup scene on disk - Project::Load and Project::Create
		// both write one when it is missing - so there is no "nothing to open" case here.
		const std::filesystem::path file = Context.project->StartupScenePath();

		// The load replaces every entity, so nothing may still be holding a handle into the old scene.
		Context.ClearSelection();

		Serialization::DeserializeScene(Context.scene, file);
	}
}
