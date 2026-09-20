#pragma once

#include "Bron/Core/Core.h"

#include <filesystem>
#include <optional>
#include <string>

namespace bron {
/// Everything needed to start a game, written as the .brongame file that sits beside the
/// runtime executable.
///
/// It lives in the engine because both halves of the export have to agree on it: the
/// editor writes one, the runtime reads one, and the two are separate executables that
/// cannot see each other's code. One struct with both directions on it is the only
/// arrangement where adding a field cannot quietly leave one side reading a default.
///
/// This is not the editor's ProjectSettings and should not grow into it. A .brn is
/// authoring state and will accumulate things that only mean something with an editor
/// attached; this is the exported subset, and a shipped game should not be reading
/// anything beyond it.
struct Manifest {
	std::string name = "Bron";

	/// Where assets live, relative to the directory holding the manifest.
	std::filesystem::path asset_directory = "Assets";

	/// The scene to start in, relative to the asset root.
	std::filesystem::path startup_scene;

	/// Reads the manifest at 'file'. Returns nothing and logs when it cannot be read,
	/// parsed, or names no scene - all three leave the caller with nothing to run, and
	/// what to do about that is the caller's decision.
	static std::optional<Manifest> Load(const std::filesystem::path& file);

	/// Writes it, creating the parent directory if it is not there. Returns false and
	/// logs on failure.
	[[nodiscard]] bool Save(const std::filesystem::path& file) const;
};
} // namespace bron
