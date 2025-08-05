//
// Created by mathijs on 8/2/25.
//

#ifndef MODELLOADER_H
#define MODELLOADER_H


#include <filesystem>


#include "Model.h"
#include "assimp/scene.h"

namespace Steve {

class ModelLoader {
public:
	static Ref<Model> loadModel(RegistryData *regData, MaterialWorkflow type, const char *modelLocation);
	static std::vector<Ref<Mesh>> processNode(RegistryData *regData, std::vector<Ref<MaterialBase>> *materials,
											  const aiNode *node, const aiScene *scene);
	static Ref<Mesh> processMesh(RegistryData *regData, const std::vector<Ref<MaterialBase>> *materials,
								 const aiMesh *aiMesh, const aiScene *scene);
	static std::vector<Ref<MaterialBase>> processPhongMaterials(const aiScene *scene,
																const std::filesystem::path &directory);
};

} // Steve

#endif //MODELLOADER_H
