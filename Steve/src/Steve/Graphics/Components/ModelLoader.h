//
// Created by mathijs on 8/2/25.
//

#ifndef MODELLOADER_H
#define MODELLOADER_H


#include <filesystem>
#include <unordered_map>


#include "Model.h"
#include "assimp/scene.h"

namespace Steve {

class ModelLoader {
public:
	static Ref<Model> loadModel(RegistryData *regData, MaterialWorkflow type, const char *modelLocation);
	/// Walks the node hierarchy. Nodes carry a transform relative to their parent, so the transform handed
	/// down here is the accumulated transform of everything above this node.
	static std::vector<Ref<Mesh>> processNode(RegistryData *regData, std::vector<Ref<MaterialBase>> *materials,
											  const aiNode *node, const aiScene *scene,
											  const aiMatrix4x4 &parentTransform);
	static Ref<Mesh> processMesh(RegistryData *regData, const std::vector<Ref<MaterialBase>> *materials,
								 const aiMesh *aiMesh, const aiScene *scene, const aiMatrix4x4 &transform);
	static std::vector<Ref<MaterialBase>> processPhongMaterials(const aiScene *scene,
																const std::filesystem::path &directory);

	/// Resolves a texture referenced by a material. The reference is either a path relative to the model
	/// directory, or - for self contained formats such as .glb - a handle ("*0") into the textures embedded
	/// in the scene itself. Returns nullptr when the texture cannot be resolved.
	/// Textures are cached per model load, so one shared by several materials is only uploaded once.
	static Ref<Texture> loadMaterialTexture(const aiScene *scene, const std::filesystem::path &directory,
											const aiString &reference,
											std::unordered_map<std::string, Ref<Texture>> &cache);
};

} // Steve

#endif //MODELLOADER_H
