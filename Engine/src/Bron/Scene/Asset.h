//
// Created by mathi on 22-9-2026.
//


#pragma once
#include "Bron/Core/UUID.h"
#include "Bron/Graphics/MaterialBase.h"
#include "Bron/Graphics/Texture.h"
#include "Bron/Graphics/VertexArray.h"
#include "Bron/Graphics/Components/Mesh.h"
#include "Bron/Physics/Components.h"
#include "Bron/Scripting/Scripting.h"

#include <glm/gtc/quaternion.hpp>

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace bron::assets {

using AssetHandle = UUID;

// A handle that is the same in every build and every project, for the assets the engine
// provides itself. Handles are 28 characters, so the name has to fit in that.
AssetHandle FixedHandle(const char* name);

// Assets that exist without a file. A scene can reference them like any other.
namespace builtin {
inline const AssetHandle kCubeMesh = FixedHandle("builtin-mesh-cube");
inline const AssetHandle kDefaultMaterial = FixedHandle("builtin-material-default");
} // namespace builtin

enum AssetType {
	kModel,
	kMesh,
	kMaterial,
	kTexture,
	kScript,
};

struct ModelImportSettings {
	MaterialWorkflow workflow = kPhong;

	// Where in the file each sub-asset came from ("mesh/0", "material/2", "texture/*0"),
	// mapped to the handle it was given. Re-importing looks the key up first, so a mesh
	// keeps its handle - and every scene pointing at it keeps working - for as long as
	// the file keeps it in the same place.
	std::map<std::string, AssetHandle> sub_assets;
};

struct AssetMetadata {
	AssetType type;

	// The file the asset is loaded from, relative to the asset root. A sub-asset has no
	// file of its own and names its parent's. Empty for built-in and memory-only assets.
	std::filesystem::path path;

	// Set for sub-assets: the model that a mesh, material or embedded texture came from.
	// Loading one means loading its parent, which produces all of them.
	std::optional<AssetHandle> parent;

	bool builtin = false;

	std::variant<std::monostate, ModelImportSettings> settings;
};

struct Asset {
	virtual ~Asset() = default;
	virtual AssetType Type() const = 0;
};

struct MeshAsset : Asset {
	static constexpr AssetType kType = kMesh;

	// Uploads the mesh straight away, so it has to be constructed on the thread that owns
	// the GL context.
	explicit MeshAsset(MeshData&& mesh_data);

	AssetType Type() const override { return kType; }

	MeshData mesh_data;

	// The mesh in kDefaultVertexLayout. Every shader that draws meshes reads from this one.
	Ref<VertexArray> vao;
	AABB aabb;
};

// How a model file puts its meshes together. This is a template: placing the model in a
// scene copies it into entities (Scene::Instantiate), and nothing reads it after that.
struct ModelAsset : Asset {
	static constexpr AssetType kType = kModel;

	struct Submesh {
		std::string name;
		AssetHandle mesh;
		AssetHandle material;
	};

	struct Node {
		std::string name;

		// Index into 'nodes'. Parents come before their children, so walking the vector
		// front to back always finds a parent already created.
		std::optional<u32> parent;

		// Relative to the parent, in the same terms as a TransformComponent.
		glm::vec3 position{0.0f};
		glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
		glm::vec3 scale{1.0f};

		std::vector<Submesh> meshes;
	};

	AssetType Type() const override { return kType; }

	// nodes[0] is the root.
	std::vector<Node> nodes;
};

struct MaterialAsset : Asset {
	static constexpr AssetType kType = kMaterial;

	AssetType Type() const override { return kType; }

	// What the renderer binds. Its textures are already resolved to Ref<Texture>.
	Ref<MaterialBase> material;

	// The texture assets it was built from. Read for dependencies and saving; the
	// Graphics side never sees these.
	std::map<TextureType, AssetHandle> textures;
};

struct TextureAsset : Asset {
	static constexpr AssetType kType = kTexture;

	AssetType Type() const override { return kType; }

	Ref<Texture> texture;
};

struct ScriptAsset : Asset {
	static constexpr AssetType kType = kScript;

	AssetType Type() const override { return kType; }

	ScriptLanguage language;
};

} // namespace bron::assets
