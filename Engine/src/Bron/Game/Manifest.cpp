#include "Bron/Game/Manifest.h"

#include "Bron/Core/Logger.h"

#include <nlohmann/json.hpp>

#include <fstream>

namespace bron {
namespace {
using json = nlohmann::json;

// Bumped when the layout changes in a way older runtimes cannot read. A game and the
// runtime shipped beside it are exported together, so this only ever catches a mismatched
// pair - which is exactly the confusing case worth naming out loud.
constexpr int kManifestVersion = 1;
} // namespace

std::optional<Manifest> Manifest::Load(const std::filesystem::path& file) {
	std::ifstream stream(file);
	if (!stream) {
		BR_CORE_ERROR("Could not open {}", file.string());
		return std::nullopt;
	}

	json document;
	try {
		stream >> document;
	} catch (const json::exception& e) {
		BR_CORE_ERROR("{} is not a valid manifest: {}", file.string(), e.what());
		return std::nullopt;
	}

	const int version = document.value("version", 0);
	if (version != kManifestVersion) {
		BR_CORE_ERROR("{} is a version {} manifest, this build reads version {}", file.string(), version,
					  kManifestVersion);
		return std::nullopt;
	}

	const json& game = document.value("game", json::object());

	Manifest manifest;
	manifest.name = game.value("name", std::string("Bron"));
	manifest.asset_directory = game.value("assetDirectory", std::string("Assets"));
	manifest.startup_scene = game.value("startupScene", std::string());

	// There is nothing sensible to put back: a runtime cannot author a scene, and
	// inventing an empty one would show a black window instead of saying why.
	if (manifest.startup_scene.empty()) {
		BR_CORE_ERROR("{} names no startup scene", file.string());
		return std::nullopt;
	}

	return manifest;
}

bool Manifest::Save(const std::filesystem::path& file) const {
	if (startup_scene.empty()) {
		// Writing one would produce a file that Load() refuses, which is a worse failure
		// than this one: it would only be discovered by running the exported game.
		BR_CORE_ERROR("Refusing to write {}: a manifest must name a startup scene", file.string());
		return false;
	}

	std::error_code error;
	std::filesystem::create_directories(file.parent_path(), error);
	if (error) {
		BR_CORE_ERROR("Could not create {}: {}", file.parent_path().string(), error.message());
		return false;
	}

	json document;
	document["version"] = kManifestVersion;
	document["game"]["name"] = name;

	// generic_string keeps the file readable and portable: a path written with Windows
	// separators would not resolve on a platform that exports the same project.
	document["game"]["assetDirectory"] = asset_directory.generic_string();
	document["game"]["startupScene"] = startup_scene.generic_string();

	std::ofstream stream(file);
	if (!stream) {
		BR_CORE_ERROR("Could not write {}", file.string());
		return false;
	}

	stream << document.dump(1, '\t') << '\n';
	return true;
}
} // namespace bron
