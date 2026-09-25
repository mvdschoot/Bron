#include "Panels/ProjectPanel.h"

#include <cstdio>

#include "Core/Icons.h"


namespace bron::editor {
using namespace ImGui;

void ProjectPanel::ImGuiContent() {
	if (!context_.HasProject()) {
		// The editor starts here when nothing has been opened before.
		TextDisabled("No project open.");
		TextDisabled("Create or open one from the File menu.");
		return;
	}

	const Project& project = *context_.project;

	Text("%s", project.Settings().name.c_str());
	TextDisabled("%s", project.AssetRoot().string().c_str());

	Separator();

	DrawSettings();

	Separator();

	if (icons::Button(icons::Id::kSave, "Save the project and the scenes it has open"))
		context_.project->Save();

	SameLine();
	AlignTextToFramePadding();
	Text("Save");
}

void ProjectPanel::DrawSettings() {
	if (!CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	ProjectSettings& settings = context_.project->Settings();

	Indent();

	char name[128];
	std::snprintf(name, sizeof(name), "%s", settings.name.c_str());
	if (InputText("Name", name, sizeof(name)))
		settings.name = name;

	char scene[260];
	std::snprintf(scene, sizeof(scene), "%s", settings.startup_scene.generic_string().c_str());
	if (InputText("Startup scene", scene, sizeof(scene)))
		settings.startup_scene = scene;

	// The asset directory is not editable here: changing it invalidates every path
	// already stored in the scenes, so it belongs in a migration, not a text field.
	BeginDisabled();
	char assets[260];
	std::snprintf(assets, sizeof(assets), "%s", settings.asset_directory.generic_string().c_str());
	InputText("Asset directory", assets, sizeof(assets));
	EndDisabled();

	Unindent();
}

} // namespace bron::editor
