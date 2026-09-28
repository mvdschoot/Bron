//
// Created by mathi on 22-9-2026.
//

#include "AssetManager.h"

#include "FontLoader.h"

#include <fstream>

#include <nlohmann/json.hpp>

#include "Bron/Core/Logger.h"
#include "Bron/Graphics/Phong/PhongMaterial.h"
#include "Bron/Scene/Components.h"
#include "Bron/Scene/ModelLoader.h"
#include "Bron/Util/Paths.h"

namespace bron::assets {

namespace {
constexpr const char* kMetaExtension = ".meta";

std::filesystem::path MetaPath(const std::filesystem::path& absolute) {
	std::filesystem::path meta = absolute;
	meta += kMetaExtension;
	return meta;
}

// A sub-asset's type is spelled out in its key, so the .meta does not have to repeat it.
std::optional<AssetType> SubAssetType(const std::string& key) {
	if (key.starts_with("mesh/"))
		return kMesh;
	if (key.starts_with("material/"))
		return kMaterial;
	if (key.starts_with("texture/"))
		return kTexture;
	return std::nullopt;
}

Ref<PhongMaterial> CreatePhong(const glm::vec3& diffuse, const glm::vec3& specular, const float shininess,
							   const float shininess_strength) {
	Ref<PhongMaterial> material = CreateRef<PhongMaterial>();
	material->Set(PhongMaterialVariables::kDiffuse, diffuse);
	material->Set(PhongMaterialVariables::kSpecular, specular);
	material->Set(PhongMaterialVariables::kShininess, shininess);
	material->Set(PhongMaterialVariables::kShininessStrength, shininess_strength);
	material->Set(PhongMaterialVariables::kAmbientFactor, 0.2f);
	return material;
}

Ref<Texture> CreateEmbeddedTexture(const ImportedTexture& imported) {
	if (imported.width == 0)
		return Texture2D::CreateFromMemory(imported.bytes.data(), static_cast<u32>(imported.bytes.size()));

	Ref<Texture> texture = Texture2D::Create(imported.width, imported.height);
	texture->SetData(const_cast<u8*>(imported.bytes.data()), static_cast<u32>(imported.bytes.size()));
	return texture;
}

Ref<FontAsset> CreateFontAsset(const ImportedFont& font) {
	Ref<FontAsset> asset = CreateRef<FontAsset>();
	asset->texture = font.texture;
	asset->font_size = font.font_size;
	for (const auto& [key, value]: font.characters) {
		asset->characters.emplace(
				key,
				FontAsset::Character{.location = value.location, .bearing = value.bearing, .advance = value.advance});
	}

	return asset;
}
} // namespace

AssetManager& AssetManager::Instance() {
	static AssetManager instance;
	return instance;
}

void AssetManager::Refresh() {
	registry_.clear();
	cache_.clear();
	by_path_.clear();

	if (!paths::HasRoots())
		return;

	std::error_code ec;
	for (const auto& entry: std::filesystem::recursive_directory_iterator(paths::AssetRoot(), ec)) {
		if (!entry.is_regular_file() || entry.path().extension() != kMetaExtension)
			continue;

		std::filesystem::path source = entry.path();

		// Removes the .meta extension, which only leaves the actual source file (e.g. "....fbx")
		source.replace_extension();

		// A .meta whose file was deleted or renamed outside the editor. Left alone rather
		// than removed, so putting the file back restores its handle.
		if (!std::filesystem::exists(source))
			continue;

		std::ifstream stream(entry.path());
		const nlohmann::json meta = nlohmann::json::parse(stream, nullptr, false);
		if (meta.is_discarded() || !meta.contains("type")) {
			BR_CORE_WARN("{} is not a valid asset .meta file", entry.path().string());
			continue;
		}
		std::string name;
		if (meta.contains("name")) {
			name = meta.at("name").get<std::string>();
		} else {
			name = source.stem().string();
		}

		Register(name, source, meta.at("type").get<AssetType>());
	}

	BR_CORE_INFO("Found {} assets under {}", registry_.size(), paths::AssetRoot().string());
}

AssetHandle AssetManager::Register(const std::string name, const std::filesystem::path& absolute, const AssetType type,
								   const std::optional<AssetHandle>& preferred) {
	const std::filesystem::path relative = paths::RelativeToAsset(absolute).lexically_normal();
	if (const auto known = by_path_.find(relative); known != by_path_.end())
		return known->second;

	AssetMetadata metadata{.name = name, .type = type, .path = relative};
	if (type == kModel)
		metadata.settings = ModelImportSettings{};
	else if (type == kFont)
		metadata.settings = FontImportSettings{.size = kImportFontSize};

	std::optional<AssetHandle> handle;
	bool write = true;

	if (std::ifstream stream(MetaPath(absolute)); stream) {
		const nlohmann::json meta = nlohmann::json::parse(stream, nullptr, false);

		// nlohmann maps an unknown enum string to the first value, so a missing type has to be
		// caught before the comparison rather than by it.
		if (!meta.is_discarded() && meta.contains("type") && meta.contains("handle") &&
			meta.at("type").get<AssetType>() == type) {
			handle = meta.at("handle").get<AssetHandle>();
			write = false;

			if (type == kModel && meta.contains("settings")) {
				const nlohmann::json& json = meta.at("settings");
				ModelImportSettings& settings = std::get<ModelImportSettings>(metadata.settings);
				settings.workflow = json.value("workflow", kPhong);
				settings.sub_assets = json.value("subAssets", std::map<std::string, AssetHandle>());
			}

			if (type == kFont && meta.contains("settings")) {
				FontImportSettings& settings = std::get<FontImportSettings>(metadata.settings);
				settings.size = meta.at("settings").value("size", settings.size);
			}
		}
	}

	// A file copied together with its .meta outside the editor brings the original's
	// handle along. The copy is the one that gives it up.
	if (handle.has_value() && registry_.contains(*handle)) {
		BR_CORE_WARN("{} has the same handle as {}; giving it a new one", relative.string(),
					 registry_.at(*handle).path.string());
		handle.reset();
		if (ModelImportSettings* settings = std::get_if<ModelImportSettings>(&metadata.settings))
			settings->sub_assets.clear();
		write = true;
	}

	if (!handle.has_value() && preferred.has_value() && !registry_.contains(*preferred))
		handle = *preferred;

	if (!handle.has_value())
		handle = AssetHandle();

	registry_[*handle] = std::move(metadata);
	by_path_[relative] = *handle;

	if (type == kModel)
		RegisterSubAssets(*handle);

	if (write)
		WriteMeta(*handle);

	return *handle;
}

void AssetManager::RegisterSubAssets(const AssetHandle& model) {
	const AssetMetadata& metadata = registry_.at(model);

	for (const auto& [key, handle]: std::get<ModelImportSettings>(metadata.settings).sub_assets) {
		const std::optional<AssetType> type = SubAssetType(key);
		if (!type.has_value()) {
			BR_CORE_WARN("{}: unknown sub-asset '{}'", metadata.path.string(), key);
			continue;
		}

		registry_[handle] = AssetMetadata{.type = *type, .path = metadata.path, .parent = model};
	}
}

void AssetManager::WriteMeta(const AssetHandle& handle) const {
	const AssetMetadata& metadata = registry_.at(handle);

	nlohmann::json meta;
	meta["name"] = metadata.name;
	meta["handle"] = handle;
	meta["type"] = metadata.type;

	if (const ModelImportSettings* settings = std::get_if<ModelImportSettings>(&metadata.settings)) {
		meta["settings"]["workflow"] = settings->workflow;
		meta["settings"]["subAssets"] = settings->sub_assets;
	}

	if (const FontImportSettings* settings = std::get_if<FontImportSettings>(&metadata.settings))
		meta["settings"]["size"] = settings->size;

	const std::filesystem::path file = MetaPath(paths::ResolveAsset(metadata.path));
	std::ofstream stream(file);
	if (!stream) {
		BR_CORE_ERROR("Could not write {}", file.string());
		return;
	}

	stream << meta.dump(1, '	');
}

std::optional<AssetHandle> AssetManager::LoadModel(const std::filesystem::path& location,
												   const MaterialWorkflow workflow) {
	const std::filesystem::path absolute = paths::ResolveAsset(location);

	const bool first_import = !std::filesystem::exists(MetaPath(absolute));
	const AssetHandle handle = Register(location.stem().string(), absolute, kModel);
	if (cache_.contains(handle))
		return handle;

	AssetMetadata& metadata = registry_.at(handle);
	ModelImportSettings& settings = std::get<ModelImportSettings>(metadata.settings);
	if (first_import)
		settings.workflow = workflow;

	std::optional<ImportedModel> imported = ModelLoader::Import(absolute);
	if (!imported.has_value())
		return std::nullopt;

	// The handle a sub-asset had last time, or a new one. Either way it is (re)registered,
	// because a sub-asset the file no longer has is simply never asked for again.
	bool new_sub_assets = false;
	auto sub_asset = [&](const std::string& key, const AssetType type) {
		auto [entry, inserted] = settings.sub_assets.try_emplace(key);
		new_sub_assets |= inserted;
		registry_[entry->second] = AssetMetadata{.type = type, .path = metadata.path, .parent = handle};
		return entry->second;
	};

	// Textures on disk are assets of their own, shared by every model that uses them.
	// Embedded ones only exist inside this file.
	std::vector<std::optional<AssetHandle>> textures(imported->textures.size());
	for (usize i = 0; i < imported->textures.size(); i++) {
		const ImportedTexture& texture = imported->textures[i];

		if (texture.embedded) {
			const AssetHandle texture_handle = sub_asset("texture/" + texture.reference, kTexture);
			const Ref<TextureAsset> asset = CreateRef<TextureAsset>();
			asset->texture = CreateEmbeddedTexture(texture);
			cache_[texture_handle] = asset;
			textures[i] = texture_handle;
		} else {
			textures[i] = LoadTexture(texture.path);
		}
	}

	std::vector<AssetHandle> materials(imported->materials.size());
	for (usize i = 0; i < imported->materials.size(); i++) {
		const ImportedMaterial& source = imported->materials[i];
		const Ref<MaterialAsset> asset = CreateRef<MaterialAsset>();

		switch (settings.workflow) {
			case kPhong: {
				const Ref<PhongMaterial> phong =
						CreatePhong(source.diffuse, source.specular, source.shininess, source.shininess_strength);

				auto attach = [&](const std::optional<u32> index, const TextureType type) {
					if (!index.has_value() || !textures[*index].has_value())
						return;
					if (const Ref<TextureAsset> texture = Get<TextureAsset>(*textures[*index])) {
						phong->AddTexture(type, texture->texture);
						asset->textures[type] = *textures[*index];
					}
				};
				attach(source.diffuse_texture, kDiffuse);
				attach(source.specular_texture, kSpecular);

				asset->material = phong;
				break;
			}
			default:
				BR_CORE_ASSERT(false, "Material workflow {} has no builder", static_cast<int>(settings.workflow));
		}

		materials[i] = sub_asset("material/" + std::to_string(i), kMaterial);
		cache_[materials[i]] = asset;
	}

	std::vector<AssetHandle> meshes(imported->meshes.size());
	for (usize i = 0; i < imported->meshes.size(); i++) {
		meshes[i] = sub_asset("mesh/" + std::to_string(i), kMesh);
		cache_[meshes[i]] = CreateRef<MeshAsset>(std::move(imported->meshes[i].data));
	}

	const Ref<ModelAsset> model = CreateRef<ModelAsset>();
	model->nodes.reserve(imported->nodes.size());
	for (const ImportedNode& source: imported->nodes) {
		ModelAsset::Node& node = model->nodes.emplace_back();
		node.name = source.name;
		node.parent = source.parent;
		node.position = source.position;
		node.rotation = source.rotation;
		node.scale = source.scale;

		for (const u32 mesh: source.meshes) {
			const u32 material = imported->meshes[mesh].material;
			node.meshes.push_back({
					.name = imported->meshes[mesh].name,
					.mesh = meshes[mesh],
					.material = material < materials.size() ? materials[material] : kNullHandle,
			});
		}
	}
	cache_[handle] = model;

	if (first_import || new_sub_assets)
		WriteMeta(handle);

	BR_CORE_INFO("Loaded model {} ({} meshes, {} materials, {} textures)", metadata.path.string(), meshes.size(),
				 materials.size(), textures.size());
	return handle;
}

std::optional<AssetHandle> AssetManager::LoadTexture(const std::filesystem::path& location) {
	const std::filesystem::path absolute = paths::ResolveAsset(location);
	if (!std::filesystem::exists(absolute)) {
		BR_CORE_WARN("Texture {} does not exist", absolute.string());
		return std::nullopt;
	}

	const AssetHandle handle = Register(location.stem().string(), absolute, kTexture);
	if (!cache_.contains(handle)) {
		const Ref<TextureAsset> asset = CreateRef<TextureAsset>();
		asset->texture = Texture2D::Create(absolute.string().c_str());
		cache_[handle] = asset;
	}

	return handle;
}

std::optional<AssetHandle> AssetManager::LoadScript(const std::filesystem::path& location,
													const ScriptLanguage language) {
	const std::filesystem::path absolute = paths::ResolveAsset(location);
	if (!std::filesystem::exists(absolute)) {
		BR_CORE_WARN("Script {} does not exist", absolute.string());
		return std::nullopt;
	}

	const AssetHandle handle = Register(location.stem().string(), absolute, kScript);
	if (!cache_.contains(handle)) {
		const Ref<ScriptAsset> asset = CreateRef<ScriptAsset>();
		asset->language = language;
		cache_[handle] = asset;
	}

	return handle;
}
std::optional<AssetHandle> AssetManager::LoadFont(const std::filesystem::path& location, const float size) {
	const std::filesystem::path absolute = paths::ResolveAsset(location);
	if (!std::filesystem::exists(absolute)) {
		BR_CORE_WARN("Font {} does not exist", absolute.string());
		return std::nullopt;
	}

	const bool first_import = !std::filesystem::exists(MetaPath(absolute));
	const AssetHandle handle = Register(location.stem().string(), absolute, kFont);
	if (cache_.contains(handle))
		return handle;

	FontImportSettings& settings = std::get<FontImportSettings>(registry_.at(handle).settings);
	if (first_import) {
		settings.size = size;
		WriteMeta(handle);
	}

	const std::optional<ImportedFont> font = FontLoader::Import(absolute, settings.size);
	if (!font.has_value()) {
		BR_CORE_ERROR("Failed to load font {}", location.string());
		return std::nullopt;
	}

	cache_[handle] = CreateFontAsset(*font);
	return handle;
}

std::optional<AssetHandle> AssetManager::Import(const std::filesystem::path& location, const AssetType type,
												const AssetHandle& handle) {
	const std::filesystem::path absolute = paths::ResolveAsset(location);
	if (!std::filesystem::exists(absolute)) {
		BR_CORE_WARN("{} does not exist", absolute.string());
		return std::nullopt;
	}

	return Register(location.stem().string(), absolute, type, handle);
}

AssetHandle AssetManager::AddMemoryAsset(const std::string name, const AssetType type, Ref<Asset> asset) {
	BR_CORE_ASSERT(asset && asset->Type() == type, "Memory asset does not match its declared type");

	const AssetHandle handle;
	registry_[handle] = AssetMetadata{.name = name, .type = type};
	cache_[handle] = std::move(asset);
	return handle;
}

std::vector<AssetHandle> AssetManager::GetAll(const AssetType type) {
	std::vector<AssetHandle> result;

	for (auto& [handle, metadata]: registry_) {
		if (metadata.type == type) {
			result.push_back(handle);
		}
	}

	return result;
}

const AssetMetadata* AssetManager::Metadata(const AssetHandle& handle) const {
	const auto entry = registry_.find(handle);
	return entry == registry_.end() ? nullptr : &entry->second;
}

std::optional<std::filesystem::path> AssetManager::SourceFile(const AssetHandle& handle) const {
	const AssetMetadata* metadata = Metadata(handle);
	if (metadata == nullptr || metadata->path.empty())
		return std::nullopt;
	return metadata->path;
}

Ref<Asset> AssetManager::GetOrLoad(const AssetHandle& handle) {
	if (const auto cached = cache_.find(handle); cached != cache_.end())
		return cached->second;

	const auto entry = registry_.find(handle);
	if (entry == registry_.end())
		return nullptr;

	// Copied, because loading writes to the registry.
	const AssetMetadata metadata = entry->second;

	if (metadata.parent.has_value()) {
		// A sub-asset: loading its model produces it, along with its siblings.
		const AssetMetadata* model = Metadata(*metadata.parent);
		if (model != nullptr)
			LoadModel(model->path);
	} else {
		switch (metadata.type) {
			case kModel:
				LoadModel(metadata.path);
				break;
			case kTexture:
				LoadTexture(metadata.path);
				break;
			case kScript:
				// The .meta does not record the language, because there is only one. When a
				// second arrives it has to be written there, the way a model's workflow is.
				LoadScript(metadata.path, kLua);
				break;
			case kFont:
				LoadFont(metadata.path);
				break;
			default:
				BR_CORE_WARN("{} is a kind of asset that cannot be loaded from a file yet", metadata.path.string());
				break;
		}
	}

	const auto loaded = cache_.find(handle);
	return loaded == cache_.end() ? nullptr : loaded->second;
}

} // namespace bron::assets
