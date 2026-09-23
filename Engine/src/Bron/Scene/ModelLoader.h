//
// Created by mathijs on 8/2/25.
//

#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Bron/Core/Core.h"
#include "Bron/Graphics/Components/Mesh.h"

struct aiMaterial;
struct aiMesh;
struct aiNode;
struct aiScene;
struct aiString;

namespace bron {

// Everything a model file contains, as plain CPU data. The pieces refer to each other by
// index into these vectors - handles, GPU uploads and entities are the asset manager's and
// the scene's business, not the parser's.
struct ImportedMesh {
	std::string name;
	MeshData data;
	u32 material = 0; // index into ImportedModel::materials
};

// Workflow neutral: whoever builds the real material decides what these mean for it.
struct ImportedMaterial {
	std::string name;
	glm::vec3 diffuse{1.0f};
	glm::vec3 specular{1.0f};
	float shininess = 5.0f;
	float shininess_strength = 1.0f;

	// Indices into ImportedModel::textures.
	std::optional<u32> diffuse_texture;
	std::optional<u32> specular_texture;
};

// Either a file next to the model, or an image stored inside it (.glb, binary .fbx).
struct ImportedTexture {
	// The reference as the model spells it: a path relative to the model's directory, or
	// an embedded handle such as "*0".
	std::string reference;

	bool embedded = false;

	// External textures: the absolute location on disk.
	std::filesystem::path path;

	// Embedded textures. A compressed image (png/jpg/...) when width is 0, otherwise raw
	// RGBA8 texels, bottom row first.
	std::vector<u8> bytes;
	u32 width = 0;
	u32 height = 0;
};

struct ImportedNode {
	std::string name;
	std::optional<u32> parent; // index into ImportedModel::nodes; parents come first

	// Relative to the parent.
	glm::vec3 position{0.0f};
	glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
	glm::vec3 scale{1.0f};

	std::vector<u32> meshes; // indices into ImportedModel::meshes
};

struct ImportedModel {
	std::vector<ImportedMesh> meshes;
	std::vector<ImportedMaterial> materials;
	std::vector<ImportedTexture> textures;
	std::vector<ImportedNode> nodes; // nodes[0] is the root
};

// Reads a model file with assimp. Makes no GL calls, so it is safe to run off the main
// thread.
class ModelLoader {
public:
	/// Returns nothing when assimp cannot read the file; the reason has been logged.
	static std::optional<ImportedModel> Import(const std::filesystem::path& location);

private:
	/// Appends 'node' and everything below it to model.nodes, parent before child.
	static void ProcessNode(const aiNode* node, std::optional<u32> parent, ImportedModel& model);
	static ImportedMesh ProcessMesh(const aiMesh* ai_mesh);
	static ImportedMaterial ProcessMaterial(const aiScene* scene, const aiMaterial* ai_material,
											const std::filesystem::path& directory, ImportedModel& model);

	/// Returns the index of the texture 'reference' names, adding it to model.textures
	/// the first time it is seen, so materials sharing a texture share the entry. Returns
	/// nothing when the reference is empty or cannot be resolved.
	static std::optional<u32> ProcessTexture(const aiScene* scene, const std::filesystem::path& directory,
											 const aiString& reference, ImportedModel& model);
};

} // namespace bron
