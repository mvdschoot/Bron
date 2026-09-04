#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "Panels/Panel.h"

namespace bron::editor {
/// Browses the files under the project root. Deliberately primitive for now: one
/// selection at a time, no drag and drop, no thumbnails. Double-clicking a folder
/// enters it; double-clicking a file goes to ActivateFile, which is the hook the rest
/// of the editor will grow into.
class FileExplorerPanel final : public Panel {
public:
	explicit FileExplorerPanel(EditorContext& context) : Panel(context) {}

	void OnImGuiRender() override;

private:
	/// What an entry is, so the list can label it and so ActivateFile knows what it was
	/// handed without looking at the extension again.
	enum EntryType { kFolder, kModelFile, kProjectFile, kSceneFile, kOther };

	struct Entry {
		EntryType type;
		std::filesystem::path path;
		/// The filename, kept as a string because that is what ImGui takes and it is
		/// drawn every frame.
		std::string name;
	};

	/// Reads current_path_ into entries_ and drops the selection, which belonged to the
	/// folder we are leaving.
	void Refresh();

	/// Moves to a folder and lists it. Anything unreadable leaves us where we are.
	void OpenFolder(const std::filesystem::path& folder);

	void DrawHeader();
	void DrawEntries();

	/// Called when a file is double-clicked. Opening scenes and importing models belong
	/// here; until they exist it only logs what was asked for.
	void ActivateFile(const Entry& entry);

	/// The folder being listed, absolute. Empty until a project is open.
	std::filesystem::path current_path_;

	/// The root the project was at when we last listed a folder. The panel outlives any
	/// one project, so a change here means we are browsing a tree that is gone.
	std::filesystem::path project_root_;

	/// current_path_ relative to the project root, for the header. Built on refresh
	/// rather than every frame.
	std::string path_label_;

	std::vector<Entry> entries_;

	/// Index into entries_, or -1 for nothing selected.
	int selected_ = -1;
};
} // namespace bron::editor
