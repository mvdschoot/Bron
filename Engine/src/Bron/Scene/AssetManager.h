//
// Created by mathi on 22-9-2026.
//


#pragma once
#include "Asset.h"

#include <filesystem>
#include <map>
#include <optional>

namespace bron::assets {

// Knows every asset in the project, hands out stable handles for them and loads them on
// first use. It knows nothing about scenes or entities: they hold handles, and resolve
// them here.
//
// Every file-backed asset has a '<file>.meta' next to it holding its handle - and, for a
// model, the handles of the meshes, materials and embedded textures inside it. That is
// what keeps a handle stored in a scene pointing at the same thing across runs.
//
// There is one for the whole process, like the asset root it resolves against: see
// paths::SetRoots, after which Refresh() has to be called.
class AssetManager {
public:
	static AssetManager& Instance();

	// Forgets every project asset and rebuilds the registry from the .meta files under
	// the asset root. Nothing is loaded until something asks for it.
	void Refresh();

	// Imports the model the first time it is seen (writing its .meta), loads it and every
	// sub-asset otherwise. 'workflow' only applies to a first import; after that the
	// .meta has the final say. Returns nothing when the file cannot be read.
	std::optional<AssetHandle> LoadModel(const std::filesystem::path& location, MaterialWorkflow workflow = kPhong);
	std::optional<AssetHandle> LoadTexture(const std::filesystem::path& location);
	std::optional<AssetHandle> LoadScript(const std::filesystem::path& location, ScriptLanguage language);
	std::optional<AssetHandle> LoadFont(const std::filesystem::path& location, float initial_size);

	// Registers an asset that has no file, e.g. a material built at runtime. It lasts until
	// the next Refresh(), so a scene saved while pointing at one will not find it again.
	AssetHandle AddMemoryAsset(AssetType type, Ref<Asset> asset);

	// The asset behind 'handle', loading it if needed. Null when the handle is unknown,
	// the file cannot be loaded, or the asset is not a T.
	template<typename T>
	Ref<T> Get(const AssetHandle& handle) {
		Ref<Asset> asset = GetOrLoad(handle);
		if (!asset || asset->Type() != T::kType)
			return nullptr;
		return std::static_pointer_cast<T>(asset);
	}

	[[nodiscard]] const AssetMetadata* Metadata(const AssetHandle& handle) const;

	// The file an asset is loaded from, relative to the asset root: its own, or its
	// parent's for a sub-asset. Nothing for built-in and memory-only assets.
	[[nodiscard]] std::optional<std::filesystem::path> SourceFile(const AssetHandle& handle) const;

private:
	AssetManager();

	Ref<Asset> GetOrLoad(const AssetHandle& handle);
	Ref<Asset> LoadBuiltin(const AssetHandle& handle);

	// Adds a file-backed asset to the registry, reading its .meta or writing a new one.
	// Returns the handle it is known by.
	AssetHandle Register(const std::filesystem::path& absolute, AssetType type);

	// Registers the sub-assets a model's .meta lists.
	void RegisterSubAssets(const AssetHandle& model);

	void WriteMeta(const AssetHandle& handle) const;

	std::map<AssetHandle, AssetMetadata> registry_;
	std::map<AssetHandle, Ref<Asset>> cache_;

	// Top-level assets only, keyed by their path relative to the asset root.
	std::map<std::filesystem::path, AssetHandle> by_path_;
};
} // namespace bron::assets
