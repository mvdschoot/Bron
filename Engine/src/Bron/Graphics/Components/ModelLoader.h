//
// Created by mathijs on 8/2/25.
//

#pragma once


#include <filesystem>
#include <unordered_map>


#include <entt/entity/registry.hpp>

#include "Bron/Graphics/MaterialBase.h"
#include "Bron/Graphics/Texture.h"
#include "Bron/Scene/Components.h"
#include "assimp/scene.h"

namespace bron {

class Scene;

class ModelLoader {
public:
	/// Loads a model into the scene. Returns the model root entity: an unparented entity whose children
	/// are one entity per mesh. The root's transform is the model centroid, each mesh's transform its own
	/// centroid relative to it.
	static entt::entity LoadModel(Scene& target, MaterialWorkflow type, const char* model_location);
	/// Walks the node hierarchy. Nodes carry a transform relative to their parent, so the transform handed
	/// down here is the accumulated transform of everything above this node.
	static std::vector<entt::entity> ProcessNode(Scene& target, std::vector<Ref<MaterialBase>>* materials,
												 const aiNode* node, const aiScene* scene,
												 const aiMatrix4x4& parent_transform);
	static entt::entity ProcessMesh(Scene& target, const std::vector<Ref<MaterialBase>>* materials,
									const aiMesh* aiMesh, const aiScene* scene, const aiMatrix4x4& transform);
	static std::vector<Ref<MaterialBase>> ProcessPhongMaterials(const aiScene* scene,
																const std::filesystem::path& directory);

	/// Resolves a texture referenced by a material. The reference is either a path relative to the model
	/// directory, or - for self contained formats such as .glb - a handle ("*0") into the textures embedded
	/// in the scene itself. Returns nullptr when the texture cannot be resolved.
	/// Textures are cached per model load, so one shared by several materials is only uploaded once.
	static Ref<Texture> LoadMaterialTexture(const aiScene* scene, const std::filesystem::path& directory,
											const aiString& reference,
											std::unordered_map<std::string, Ref<Texture>>& cache);
};

} // namespace bron
