#include "Panels/FileExplorerPanel.h"

#include <algorithm>
#include <array>

#include "Bron/Util/Paths.h"
#include "Bron/Util/Util.h"

namespace bron::editor {
using namespace ImGui;

namespace {
constexpr std::array kModelExtensions{".fbx", ".glb", ".gltf", ".obj"};
} // namespace

void FileExplorerPanel::OnImGuiRender() {
	Begin("File Explorer");

	if (!context_.HasProject()) {
		TextDisabled("No project open.");
		End();
		return;
	}

	// The panel outlives the project, so pick up a root that has been opened or swapped
	// since the last frame instead of listing a tree that no longer exists.
	if (paths::ProjectRoot() != project_root_) {
		project_root_ = paths::ProjectRoot();
		OpenFolder(project_root_);
	}

	DrawHeader();
	Separator();
	DrawEntries();

	End();
}

void FileExplorerPanel::DrawHeader() {
	// Nowhere to go up to from the root: the project owns the tree, and browsing out of
	// it would show files no path in a scene can name.
	const bool at_root = current_path_ == project_root_;

	BeginDisabled(at_root);
	if (Button("Up"))
		OpenFolder(current_path_.parent_path());
	EndDisabled();

	SameLine();
	Text("%s", path_label_.c_str());
}

void FileExplorerPanel::DrawEntries() {
	if (!BeginChild("Entries"))
		return;

	if (entries_.empty())
		TextDisabled("Empty folder.");

	for (int i = 0; i < static_cast<int>(entries_.size()); ++i) {
		const Entry& entry = entries_[i];

		// Folders read as folders without an icon set to draw from yet.
		const std::string label = entry.type == kFolder ? entry.name + "/" : entry.name;

		// The index is the id: two entries in one folder cannot share a name.
		PushID(i);
		if (Selectable(label.c_str(), i == selected_, ImGuiSelectableFlags_AllowDoubleClick)) {
			selected_ = i;

			if (IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				// Entering a folder rebuilds entries_, so nothing below may touch the
				// entry afterwards - hence the copy of the path.
				if (entry.type == kFolder)
					OpenFolder(std::filesystem::path(entry.path));
				else
					ActivateFile(entry);

				PopID();
				break;
			}
		}
		PopID();
	}

	EndChild();
}

void FileExplorerPanel::ActivateFile(const Entry& entry) {
	// TODO: open scenes, import models, hand the rest to the OS.
	BR_APP_INFO("File explorer: activated {}", entry.path.string());
}

void FileExplorerPanel::OpenFolder(const std::filesystem::path& folder) {
	std::error_code ec;
	if (!std::filesystem::is_directory(folder, ec)) {
		BR_APP_WARN("File explorer: {} is not a folder", folder.string());
		return;
	}

	current_path_ = folder;
	Refresh();
}

void FileExplorerPanel::Refresh() {
	entries_.clear();
	selected_ = -1;

	std::error_code ec;
	for (const std::filesystem::directory_entry& file: std::filesystem::directory_iterator(current_path_, ec)) {
		// extension() keeps the dot, so the constants above carry one too.
		const std::string extension = ToLowerCase(file.path().extension().string());

		EntryType type;
		if (file.is_directory())
			type = kFolder;
		else if (extension == ".brn")
			type = kProjectFile;
		else if (extension == ".scn")
			type = kSceneFile;
		else if (std::ranges::find(kModelExtensions, extension) != kModelExtensions.end())
			type = kModelFile;
		else
			type = kOther;

		entries_.emplace_back(type, file.path(), file.path().filename().string());
	}

	// An unreadable folder lists as empty rather than throwing; the panel still shows
	// where it is and the Up button still works.
	if (ec)
		BR_APP_WARN("File explorer: cannot read {} ({})", current_path_.string(), ec.message());

	// Folders first, then by name, so the listing does not reorder itself between runs
	// the way directory_iterator may.
	std::ranges::sort(entries_, [](const Entry& a, const Entry& b) {
		const bool a_folder = a.type == kFolder;
		const bool b_folder = b.type == kFolder;
		if (a_folder != b_folder)
			return a_folder;

		return ToLowerCase(a.name) < ToLowerCase(b.name);
	});

	const std::filesystem::path relative = std::filesystem::relative(current_path_, project_root_, ec);
	path_label_ = (ec || relative.empty() || relative == ".") ? "/" : "/" + relative.generic_string();
}

} // namespace bron::editor
