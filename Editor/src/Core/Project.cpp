#include "Core/Project.h"

#include <fstream>

#include <nlohmann/json.hpp>

#include "Bron/Core/Logger.h"
#include "Bron/Scene/Scene.h"
#include "Bron/Scene/Serialization/Serialization.h"
#include "Bron/Util/Paths.h"

namespace bron::editor {
namespace {
using json = nlohmann::json;

// Bumped when the .brn layout changes in a way older builds cannot read.
constexpr int kProjectVersion = 1;

// Where a project that has lost its scene gets one back.
const char* kDefaultScene = "Scenes/Main.json";
} // namespace

Scope<Project> Project::Load(const std::filesystem::path& file) {
	std::ifstream stream(file);
	if (!stream) {
		BR_CORE_ERROR("Could not open project {}", file.string());
		return nullptr;
	}

	json document;
	try {
		stream >> document;
	} catch (const json::exception& e) {
		BR_CORE_ERROR("{} is not a valid project file: {}", file.string(), e.what());
		return nullptr;
	}

	const int version = document.value("version", 0);
	if (version != kProjectVersion) {
		BR_CORE_ERROR("{} is a version {} project, this build reads version {}", file.string(), version,
					  kProjectVersion);
		return nullptr;
	}

	auto project = Scope<Project>(new Project());
	project->file_ = file;
	project->directory_ = file.parent_path();

	const json& settings = document.value("project", json::object());
	project->settings_.name = settings.value("name", file.stem().string());
	project->settings_.asset_directory = settings.value("assetDirectory", std::string("Assets"));
	project->settings_.startup_scene = settings.value("startupScene", std::string(kDefaultScene));

	if (project->settings_.startup_scene.empty()) {
		// Older or hand-edited files can name no scene. The invariant is worth more than
		// the file's word, so one is put back and written out.
		BR_CORE_WARN("{} names no scene; adding {}.", file.string(), kDefaultScene);
		project->settings_.startup_scene = kDefaultScene;
		project->Save();
	}

	project->EnsureStartupScene();

	return project;
}

Scope<Project> Project::Create(const std::filesystem::path& file, const std::string& name) {
	auto project = Scope<Project>(new Project());
	project->file_ = file;
	project->directory_ = file.parent_path();
	project->settings_.name = name;

	std::error_code error;
	std::filesystem::create_directories(project->AssetRoot(), error);
	if (error) {
		BR_CORE_ERROR("Could not create {}: {}", project->AssetRoot().string(), error.message());
		return nullptr;
	}

	// A new project is usable straight away, which means it comes with a scene.
	project->EnsureStartupScene();

	if (!project->Save())
		return nullptr;

	return project;
}

bool Project::Save() const {
	json document;
	document["version"] = kProjectVersion;
	document["project"]["name"] = settings_.name;

	// generic_string keeps the file readable and portable across platforms.
	document["project"]["assetDirectory"] = settings_.asset_directory.generic_string();
	document["project"]["startupScene"] = settings_.startup_scene.generic_string();

	std::ofstream stream(file_);
	if (!stream) {
		BR_CORE_ERROR("Could not write {}", file_.string());
		return false;
	}

	stream << document.dump(1, '\t') << '\n';
	return true;
}

void Project::MakeActive() const { paths::SetAssetRoot(AssetRoot()); }

std::filesystem::path Project::AssetRoot() const { return (directory_ / settings_.asset_directory).lexically_normal(); }

std::filesystem::path Project::Resolve(const std::filesystem::path& relative) const { return AssetRoot() / relative; }

std::filesystem::path Project::StartupScenePath() const { return Resolve(settings_.startup_scene); }

void Project::EnsureStartupScene() const {
	const std::filesystem::path file = StartupScenePath();
	if (std::filesystem::exists(file))
		return;

	std::error_code error;
	std::filesystem::create_directories(file.parent_path(), error);
	if (error) {
		BR_CORE_ERROR("Could not create {}: {}", file.parent_path().string(), error.message());
		return;
	}

	// Empty apart from the root every scene has: what goes in it is the user's business.
	const Scene initial;

	Serialization::SerializeScene(initial, file);
	BR_CORE_INFO("Created the initial scene {}", file.string());
}
} // namespace bron::editor
