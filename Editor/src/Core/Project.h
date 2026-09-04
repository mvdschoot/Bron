#pragma once

#include <filesystem>
#include <memory>
#include <string>

namespace Bron::Editor
{
	/// Project scope: the settings everyone working on a project shares, written into the
	/// .brn file and meant to be version controlled. Anything that describes the person
	/// using the editor belongs in Preferences; anything that is only true of one machine's
	/// view of this project belongs in its session, not here.
	struct ProjectSettings
	{
		std::string name = "Untitled";

		/// Where assets live, relative to the directory holding the .brn file.
		std::filesystem::path assetDirectory = "Assets";

		/// Scene opened when the project is, relative to the asset root. Never empty: a
		/// project always has at least one scene. When there are several this stays the
		/// one opened at launch.
		std::filesystem::path startupScene = "Scenes/Main.json";
	};

	/// An open project. It owns the asset root - Bron::Paths only knows how to resolve a
	/// path against whatever root it was last given, and this is what gives it one. There is
	/// no default or built-in project: with none open the editor simply has no asset root.
	class Project
	{
	public:
		/// Reads a .brn file. Returns null and logs when it cannot be read or parsed.
		static std::unique_ptr<Project> Load(const std::filesystem::path& file);

		/// Creates a project at 'file', along with its asset directory and its first scene.
		/// Returns null on failure.
		static std::unique_ptr<Project> Create(const std::filesystem::path& file, const std::string& name);

		/// Writes the .brn file.
		bool Save() const;

		/// Points Bron::Paths at this project's asset root. Call when it becomes the open one.
		void MakeActive() const;

		const std::filesystem::path& File() const { return mFile; }

		/// What assetDirectory and the .brn file are relative to.
		const std::filesystem::path& Directory() const { return mDirectory; }

		std::filesystem::path AssetRoot() const;

		/// Absolute location of a path stored relative to the asset root.
		std::filesystem::path Resolve(const std::filesystem::path& relative) const;

		/// The scene to open with, absolute. Never empty, and Load() and Create() have both
		/// guaranteed the file is there.
		std::filesystem::path StartupScenePath() const;

		/// Writes the startup scene if it is not on disk. This is what makes "a project
		/// always has at least one scene" true of the files, not just of the settings.
		void EnsureStartupScene() const;

		ProjectSettings& Settings() { return mSettings; }
		const ProjectSettings& Settings() const { return mSettings; }

	private:
		// Only the factories above make a Project, so an instance always has a valid root.
		Project() = default;

		std::filesystem::path mFile;       // The .brn.
		std::filesystem::path mDirectory;  // The directory holding the .brn.

		ProjectSettings mSettings;
	};
}
