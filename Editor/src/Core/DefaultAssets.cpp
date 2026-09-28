#include "Core/DefaultAssets.h"

#include "EditorDefaults.h"

#include "Bron/Core/Logger.h"
#include "Bron/Scene/AssetManager.h"
#include "Bron/Util/Paths.h"

#include <fstream>
#include <system_error>

namespace bron::editor::defaults {
namespace {
// Where the defaults go, relative to the asset root.
constexpr const char* kDirectory = "Bron";

struct DefaultAsset {
	const char* file;
	const embedded::defaults::Resource& contents;
	assets::AssetType type;
	const assets::AssetHandle& handle;
};

constexpr DefaultAsset kDefaults[] = {
		{"DefaultFont.ttf", embedded::defaults::kDefaultFont, assets::kFont, kFont},
};

bool Write(const std::filesystem::path& file, const embedded::defaults::Resource& contents) {
	std::error_code error;
	std::filesystem::create_directories(file.parent_path(), error);
	if (error) {
		BR_APP_ERROR("Could not create {}: {}", file.parent_path().string(), error.message());
		return false;
	}

	std::ofstream stream(file, std::ios::binary);
	if (!stream) {
		BR_APP_ERROR("Could not write {}", file.string());
		return false;
	}

	stream.write(reinterpret_cast<const char*>(contents.data), static_cast<std::streamsize>(contents.size));
	return true;
}
} // namespace

void Seed(const std::filesystem::path& asset_root) {
	for (const DefaultAsset& asset: kDefaults) {
		const std::filesystem::path relative = std::filesystem::path(kDirectory) / asset.file;
		const std::filesystem::path absolute = paths::ResolveAsset(relative);

		if (!std::filesystem::exists(absolute)) {
			if (!Write(absolute, asset.contents))
				continue;
			BR_APP_INFO("Added default asset {}", relative.generic_string());
		}

		// Registers it under the fixed handle and writes the .meta, unless Refresh() found
		// a .meta already - that one wins, as it does for any asset.
		assets::AssetManager::Instance().Import(relative, asset.type, asset.handle);
	}
}
} // namespace bron::editor::defaults
