//
// Created by mathijs on 8/2/25.
//

#include "ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/scene.h>
#include "assimp/postprocess.h"

#include <magic_enum/magic_enum.hpp>

#include <filesystem>

#include "Steve/Graphics/Phong/PhongMaterial.h"

namespace Steve {

	Ref<Model> ModelLoader::loadModel(RegistryData *regData, MaterialWorkflow type, const char *modelLocation) {
		// Assimp load model
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(modelLocation, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

		std::string error = "ERROR::ASSIMP::";
		error.append(importer.GetErrorString());
		CORE_ASSERT(scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode, error)

		// Get the directory of the model, because it will contain the textures.
		std::filesystem::path parentDirectory = std::filesystem::path(modelLocation).parent_path();

		// Set of materials, so that materials can be reused.
		std::vector<Ref<MaterialBase>> materials;
		switch (type) {
			case MaterialWorkflow::PHONG:
				materials = processPhongMaterials(scene, parentDirectory.string().c_str());
				break;
			default:
				CORE_ASSERT(false, "This MaterialWorkflow does not exist ({}).", magic_enum::enum_name(type));
		}

		std::vector<Ref<Mesh>> meshes = processNode(regData, &materials, scene->mRootNode, scene);

		// Create the model
		auto model = Ref<Model>(new Model(regData, std::move(meshes)));

		// Set model centroid & adapt Mesh centroids
		const glm::vec3 model_centroid = model->GetCentroid();
		for (const Ref<Mesh>& mesh: model->GetMeshes()) {
			mesh->GetComponent<TransformComponent>()->Position -= model_centroid;
			mesh->parent = model.get();
		}
		model->GetComponent<TransformComponent>()->Position = model_centroid;

		return model;
	}

	std::vector<Ref<Mesh>> ModelLoader::processNode(RegistryData *regData, std::vector<Ref<MaterialBase>> *materials,
													const aiNode *node, const aiScene *scene) {
		CH_PROFILE_FUNCTION();

		std::vector<Ref<Mesh>> meshes;

		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			Ref<Mesh> addedMesh = processMesh(regData, materials, mesh, scene);
			meshes.push_back(addedMesh);
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			std::vector<Ref<Mesh>> addedMeshes = processNode(regData, materials, node->mChildren[i], scene);
			meshes.insert(meshes.end(), addedMeshes.begin(), addedMeshes.end());
		}

		return meshes;
	}

	Ref<Mesh> ModelLoader::processMesh(RegistryData *regData, const std::vector<Ref<MaterialBase>> *materials,
									   const aiMesh *aiMesh, const aiScene *scene) {
		CH_PROFILE_FUNCTION();

		// First figure out the number of vertices in the mesh
		u32 num_indices = 0;
		for (u32 x = 0; x < aiMesh->mNumFaces; x++) {
			num_indices += aiMesh->mFaces[x].mNumIndices;
		}

		// Initialize the mesh data struct
		MeshData meshData;
		meshData.positions = std::vector<glm::vec3>(aiMesh->mNumVertices);
		meshData.indices = std::vector<u32>(num_indices);
		if (aiMesh->HasNormals()) {
			meshData.normals = std::vector<glm::vec3>(aiMesh->mNumVertices);
		}
		if (aiMesh->HasTangentsAndBitangents()) {
			meshData.tangents = std::vector<glm::vec3>(aiMesh->mNumVertices);
		}
		if (aiMesh->HasTextureCoords(0)) {
			meshData.uvs = std::vector<glm::vec2>(aiMesh->mNumVertices);
		}

		// We calculate the centroid of the mesh, to subtract from the vertex coordinates, and set the mesh
		// TransformComponent.
		const glm::vec3 centroid = Mesh::FindCentroid(reinterpret_cast<glm::vec3 *>(aiMesh->mVertices), aiMesh->mNumVertices);

		for (u32 i = 0; i < aiMesh->mNumVertices; i++) {
			// Position
			meshData.positions[i] = glm::vec3(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z);
			meshData.positions[i] -= centroid;

			// Normals
			if (!meshData.normals.has_value()) {
				meshData.normals.value()[i] = glm::vec3(1.0f);
			} else {
				meshData.normals.value()[i].x = aiMesh->mNormals[i].x;
				meshData.normals.value()[i].y = aiMesh->mNormals[i].y;
				meshData.normals.value()[i].z = aiMesh->mNormals[i].z;
			}

			// UV coordinates
			if (!meshData.uvs.has_value())
				meshData.uvs.value()[i] = {0.0f, 0.0f};
			else
				meshData.uvs.value()[i] = {aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y};
		}

		// Set the indices
		u32 i = 0;
		for (unsigned int x = 0; x < aiMesh->mNumFaces; x++) {
			// ASSUMES THE INDICES ARE 4 BYTE UNSIGNED INTS.
			meshData.indices.insert(meshData.indices.begin() + i, aiMesh->mFaces[x].mIndices,
									aiMesh->mFaces[x].mIndices + aiMesh->mFaces[x].mNumIndices);
			i += aiMesh->mFaces[x].mNumIndices;
		}

		// Create mesh
		const Ref<MaterialBase>& meshMaterial = (*materials)[aiMesh->mMaterialIndex];
		Ref<Mesh> mesh = createRef<Mesh>(regData, std::move(meshData), meshMaterial);

		// Set the position of the mesh
		mesh->GetComponent<TransformComponent>()->Position = centroid;

		return mesh;
	}

	std::vector<Ref<MaterialBase>> ModelLoader::processPhongMaterials(const aiScene *scene,
																	  const std::filesystem::path &directory) {
		std::vector<Ref<MaterialBase>> materials(scene->mNumMaterials);

		for (int x = 0; x < scene->mNumMaterials; x++) {
			aiMaterial* material = scene->mMaterials[x];
			Ref<PhongMaterial> phongMaterial = createRef<PhongMaterial>();

			// First extract and store the textures
			aiString dif, spec, norm;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &dif);
			material->GetTexture(aiTextureType_SPECULAR, 0, &spec);
			// material->GetTexture(aiTextureType_HEIGHT, 0, &norm);

			if (dif.length != 0) {
				phongMaterial->AddTexture(PhongMaterialTextureTypes::Diffuse, Texture2D::Create((directory / dif.C_Str()).c_str()));
			}
			if (spec.length != 0) {
				phongMaterial->AddTexture(PhongMaterialTextureTypes::Specular, Texture2D::Create((directory / spec.C_Str()).c_str()));
			}
			// if (norm.length != 0) {
			// 	phongMaterial->AddTexture(PhongMaterialTextureTypes::Normal, Texture2D::Create((directory / norm.C_Str()).c_str()));
			// }

			// Second, extract the misc phong-related variables.
			// Retrieve shininess
			if (float Shininess; material->Get(AI_MATKEY_SHININESS, Shininess) == aiReturn_SUCCESS)
				phongMaterial->Set(PhongMaterialVariables::Shininess, Shininess);
			else
				phongMaterial->Set(PhongMaterialVariables::Shininess, 5.0f);

			// Retrieve shininess strength
			if (float ShininessStrength;
				material->Get(AI_MATKEY_SHININESS_STRENGTH, ShininessStrength) == aiReturn_SUCCESS)
				phongMaterial->Set(PhongMaterialVariables::ShininessStrength, ShininessStrength);
			else
				phongMaterial->Set(PhongMaterialVariables::ShininessStrength, 1.0f);

			// Todo: Ambient factor
			phongMaterial->Set(PhongMaterialVariables::AmbientFactor, 0.2);

			// Retrieve diffuse
			if (aiColor3D Diffuse; material->Get(AI_MATKEY_COLOR_DIFFUSE, Diffuse) == aiReturn_SUCCESS)
				phongMaterial->Set(PhongMaterialVariables::Diffuse, Diffuse);
			else
				phongMaterial->Set(PhongMaterialVariables::Diffuse, 1.0f);

			// Retrieve specular
			if (aiColor3D Specular; material->Get(AI_MATKEY_COLOR_SPECULAR, Specular) == aiReturn_SUCCESS)
				phongMaterial->Set(PhongMaterialVariables::Specular, Specular);
			else
				phongMaterial->Set(PhongMaterialVariables::Specular, 1.0f);

			materials[x] = phongMaterial;
		}

		return materials;
	}


} // Steve