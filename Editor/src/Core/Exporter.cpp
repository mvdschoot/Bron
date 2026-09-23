#include "Core/Exporter.h"

#include "Bron/Core/Logger.h"
#include "Bron/Core/Platform.h"
#include "Bron/Game/Manifest.h"
#include "Bron/Scene/AssetDependencies.h"
#include "Bron/Scene/Serialization/Serialization.h"
#include "Bron/Util/Paths.h"

#include <system_error>

namespace bron::editor {
namespace {
// Where assets go inside the export, and what the manifest's assetDirectory says. Fixed
// rather than copied from the project: the exported layout is the runtime's business,
// and a project that keeps its assets somewhere unusual should not force that on it.
constexpr const char* k_export_asset_directory = "Assets";

// The prebuilt runtime, which ships beside the editor and is copied into every export.
#ifdef BR_PLATFORM_WINDOWS
constexpr const char* k_runtime_executable = "BronRuntime.exe";
#else
constexpr const char* k_runtime_executable = "BronRuntime";
#endif

/// Copies 'from' to 'to', creating the directories above it. Overwrites, because
/// exporting twice into the same folder is the normal way to use this.
bool CopyFile(const std::filesystem::path& from, const std::filesystem::path& to) {
	std::error_code error;
	std::filesystem::create_directories(to.parent_path(), error);
	if (error) {
		BR_APP_ERROR("Could not create {}: {}", to.parent_path().string(), error.message());
		return false;
	}

	std::filesystem::copy_file(from, to, std::filesystem::copy_options::overwrite_existing, error);
	if (error) {
		BR_APP_ERROR("Could not copy {}: {}", from.string(), error.message());
		return false;
	}

	return true;
}

/// The runtime executable, and anything shipped beside it that it needs to start. A
/// statically linked build has no DLLs at all; taking whatever is there keeps the export
/// correct if that ever stops being true.
bool CopyRuntime(const std::filesystem::path& destination) {
	const std::filesystem::path editor_directory = ExecutableDirectory();
	const std::filesystem::path runtime = editor_directory / k_runtime_executable;

	if (!std::filesystem::exists(runtime)) {
		// Worth naming precisely: the usual cause is an editor started from a build that
		// never built the runtime target, and the exported folder would look complete.
		BR_APP_ERROR("No {} beside the editor at {}. Build the runtime and export again.", k_runtime_executable,
					 editor_directory.string());
		return false;
	}

	if (!CopyFile(runtime, destination / k_runtime_executable))
		return false;

	std::error_code error;
	for (const auto& entry: std::filesystem::directory_iterator(editor_directory, error)) {
		if (entry.is_regular_file() && entry.path().extension() == ".dll")
			CopyFile(entry.path(), destination / entry.path().filename());
	}

	return true;
}

/// Copies every asset the scene depends on into the export, keeping the layout it has
/// under the asset root - that layout is what the paths stored in the scene mean.
bool CopyAssets(const Scene& scene, const std::filesystem::path& asset_destination) {
	bool complete = true;

	for (const std::filesystem::path& relative: CollectAssets(scene)) {
		// CollectAssets hands back an absolute path for an asset that lives outside the
		// asset root, because there is no way to express it relative to one. There is
		// nowhere to put such a file in the export either, so it is named and skipped
		// rather than quietly dropped.
		if (relative.is_absolute()) {
			BR_APP_ERROR("{} is outside the asset directory and cannot be exported. Move it inside and re-import it.",
						 relative.string());
			complete = false;
			continue;
		}

		const std::filesystem::path source = paths::ResolveAsset(relative);
		if (!std::filesystem::exists(source)) {
			BR_APP_ERROR("{} is referenced by the scene but is not on disk", source.string());
			complete = false;
			continue;
		}

		complete &= CopyFile(source, asset_destination / relative);
	}

	return complete;
}
} // namespace

bool ExportGame(const Project& project, const Scene& scene, const std::filesystem::path& destination) {
	const ProjectSettings& settings = project.Settings();
	const std::filesystem::path assets = destination / k_export_asset_directory;

	std::error_code error;
	std::filesystem::create_directories(assets, error);
	if (error) {
		BR_APP_ERROR("Could not create {}: {}", assets.string(), error.message());
		return false;
	}

	bool complete = CopyRuntime(destination);
	complete &= CopyAssets(scene, assets);

	// From memory, not copied from disk: an export has to match what is on screen, and a
	// scene saved a minute ago is not the same claim.
	const std::filesystem::path scene_file = assets / settings.startup_scene;
	std::filesystem::create_directories(scene_file.parent_path(), error);
	Serialization::SerializeScene(scene, scene_file);
	if (!std::filesystem::exists(scene_file)) {
		BR_APP_ERROR("Could not write the startup scene to {}", scene_file.string());
		complete = false;
	}

	Manifest manifest;
	manifest.name = settings.name;

	// Both relative, and both to a different thing: assetDirectory to the folder holding
	// the manifest, startupScene to the asset root. See the fields on Manifest.
	manifest.asset_directory = k_export_asset_directory;
	manifest.startup_scene = settings.startup_scene;

	if (!manifest.Save(destination / "game.brongame"))
		return false;

	if (!complete) {
		BR_APP_ERROR("Exported {} to {} with errors; it may not run.", settings.name, destination.string());
		return false;
	}

	BR_APP_INFO("Exported {} to {}", settings.name, destination.string());
	return true;
}
} // namespace bron::editor
