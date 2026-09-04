#include "Core/Project.h"

#include <fstream>

#include <nlohmann/json.hpp>

#include "Bron/Core/Logger.h"
#include "Bron/Scene/Scene.h"
#include "Bron/Scene/Serialization/Serialization.h"
#include "Bron/Util/Paths.h"

namespace Bron::Editor
{
	namespace
	{
		using json = nlohmann::json;

		// Bumped when the .brn layout changes in a way older builds cannot read.
		constexpr int kProjectVersion = 1;

		// Where a project that has lost its scene gets one back.
		const char* kDefaultScene = "Scenes/Main.json";
	}

	std::unique_ptr<Project> Project::Load(const std::filesystem::path& file)
	{
		std::ifstream stream(file);
		if (!stream)
		{
			CORE_ERROR("Could not open project {}", file.string());
			return nullptr;
		}

		json document;
		try
		{
			stream >> document;
		}
		catch (const json::exception& e)
		{
			CORE_ERROR("{} is not a valid project file: {}", file.string(), e.what());
			return nullptr;
		}

		const int version = document.value("version", 0);
		if (version != kProjectVersion)
		{
			CORE_ERROR("{} is a version {} project, this build reads version {}",
					   file.string(), version, kProjectVersion);
			return nullptr;
		}

		auto project = std::unique_ptr<Project>(new Project());
		project->mFile = file;
		project->mDirectory = file.parent_path();

		const json& settings = document.value("project", json::object());
		project->mSettings.name = settings.value("name", file.stem().string());
		project->mSettings.assetDirectory = settings.value("assetDirectory", std::string("Assets"));
		project->mSettings.startupScene = settings.value("startupScene", std::string(kDefaultScene));

		if (project->mSettings.startupScene.empty())
		{
			// Older or hand-edited files can name no scene. The invariant is worth more than
			// the file's word, so one is put back and written out.
			CORE_WARN("{} names no scene; adding {}.", file.string(), kDefaultScene);
			project->mSettings.startupScene = kDefaultScene;
			project->Save();
		}

		project->EnsureStartupScene();

		return project;
	}

	std::unique_ptr<Project> Project::Create(const std::filesystem::path& file, const std::string& name)
	{
		auto project = std::unique_ptr<Project>(new Project());
		project->mFile = file;
		project->mDirectory = file.parent_path();
		project->mSettings.name = name;

		std::error_code error;
		std::filesystem::create_directories(project->AssetRoot(), error);
		if (error)
		{
			CORE_ERROR("Could not create {}: {}", project->AssetRoot().string(), error.message());
			return nullptr;
		}

		// A new project is usable straight away, which means it comes with a scene.
		project->EnsureStartupScene();

		if (!project->Save())
			return nullptr;

		return project;
	}

	bool Project::Save() const
	{
		json document;
		document["version"] = kProjectVersion;
		document["project"]["name"] = mSettings.name;

		// generic_string keeps the file readable and portable across platforms.
		document["project"]["assetDirectory"] = mSettings.assetDirectory.generic_string();
		document["project"]["startupScene"] = mSettings.startupScene.generic_string();

		std::ofstream stream(mFile);
		if (!stream)
		{
			CORE_ERROR("Could not write {}", mFile.string());
			return false;
		}

		stream << document.dump(1, '\t') << '\n';
		return true;
	}

	void Project::MakeActive() const
	{
		Paths::SetAssetRoot(AssetRoot());
	}

	std::filesystem::path Project::AssetRoot() const
	{
		return (mDirectory / mSettings.assetDirectory).lexically_normal();
	}

	std::filesystem::path Project::Resolve(const std::filesystem::path& relative) const
	{
		return AssetRoot() / relative;
	}

	std::filesystem::path Project::StartupScenePath() const
	{
		return Resolve(mSettings.startupScene);
	}

	void Project::EnsureStartupScene() const
	{
		const std::filesystem::path file = StartupScenePath();
		if (std::filesystem::exists(file))
			return;

		std::error_code error;
		std::filesystem::create_directories(file.parent_path(), error);
		if (error)
		{
			CORE_ERROR("Could not create {}: {}", file.parent_path().string(), error.message());
			return;
		}

		// Empty apart from the root every scene has: what goes in it is the user's business.
		const Scene initial;

		Serialization::SerializeScene(initial, file);
		CORE_INFO("Created the initial scene {}", file.string());
	}
}
