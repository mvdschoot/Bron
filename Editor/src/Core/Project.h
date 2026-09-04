//
// Created by mathi on 3-9-2026.
//


#pragma once
#include <filesystem>
#include <string>

namespace Bron::Editor {
	class Project {
	public:
		Project(const std::filesystem::path &path);
	private:
		std::filesystem::path projectPath;
		std::filesystem::path projectFile;
	};
}
