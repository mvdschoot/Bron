#include "Panels/ProjectPanel.h"

#include "Core/Icons.h"

#include "imgui_stdlib.h"


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

	InputText("Name", &settings.name);

	// A path has no std::string to edit in place, so it goes through a copy.
	std::string scene = settings.startup_scene.generic_string();
	if (InputText("Startup scene", &scene))
		settings.startup_scene = scene;

	// The asset directory is not editable here: changing it invalidates every path
	// already stored in the scenes, so it belongs in a migration, not a text field.
	BeginDisabled();
	std::string assets = settings.asset_directory.generic_string();
	InputText("Asset directory", &assets);
	EndDisabled();

	Unindent();
}

} // namespace bron::editor
