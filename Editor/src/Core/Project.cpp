//
// Created by mathi on 3-9-2026.
//

#include "Project.h"

Bron::Editor::Project::Project(const std::filesystem::path &path) {
	projectPath = path.root_directory();
	projectFile = projectPath.filename();
}
