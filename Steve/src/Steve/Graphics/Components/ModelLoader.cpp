//
// Created by mathijs on 8/2/25.
//

#include "ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/scene.h>
#include "assimp/postprocess.h"

#include <magic_enum/magic_enum.hpp>

#include <algorithm>
#include <filesystem>

#include "Steve/Graphics/Phong/PhongMaterial.h"
#include "Steve/Graphics/Texture.h"
#include "Steve/Scene/Scene.h"

namespace Steve {

	// Average of the child mesh positions, i.e. the centroid of the model as a whole.
	static glm::vec3 ModelCentroid(entt::registry &reg, const std::vector<entt::entity> &meshes) {
		if (meshes.empty())
			return glm::vec3(0.0f);

		glm::vec3 res(0.0f);

		for (const entt::entity mesh : meshes) {
			res += reg.get<TransformComponent>(mesh).Position;
		}

		return res / static_cast<float>(meshes.size());
	}

	entt::entity ModelLoader::loadModel(Scene &target, MaterialWorkflow type, const char *modelLocation) {
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

		std::vector<entt::entity> meshes = processNode(target, &materials, scene->mRootNode, scene, aiMatrix4x4());

		// Create the model root. It stays unparented; the caller decides where it goes in the scene.
		const entt::entity model = target.CreateEntity(std::filesystem::path(modelLocation).stem().string());

		// The model sits at the centroid of its meshes, and each mesh is placed relative to that.
		const glm::vec3 model_centroid = ModelCentroid(target.reg, meshes);
		for (const entt::entity mesh : meshes) {
			target.reg.get<TransformComponent>(mesh).Position -= model_centroid;
			target.AddChild(model, mesh);
		}
		target.reg.get<TransformComponent>(model).Position = model_centroid;

		return model;
	}

	std::vector<entt::entity> ModelLoader::processNode(Scene &target, std::vector<Ref<MaterialBase>> *materials,
													   const aiNode *node, const aiScene *scene,
													   const aiMatrix4x4 &parentTransform) {
		CH_PROFILE_FUNCTION();

		std::vector<entt::entity> meshes;

		// A node positions, rotates and scales its meshes relative to its parent. Formats that keep their
		// scene graph intact (glTF/.glb especially) put a large part of the placement here instead of in the
		// vertex data, so it has to be accumulated on the way down and applied to the vertices.
		const aiMatrix4x4 transform = parentTransform * node->mTransformation;

		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(target, materials, mesh, scene, transform));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			std::vector<entt::entity> addedMeshes = processNode(target, materials, node->mChildren[i], scene, transform);
			meshes.insert(meshes.end(), addedMeshes.begin(), addedMeshes.end());
		}

		return meshes;
	}

	entt::entity ModelLoader::processMesh(Scene &target, const std::vector<Ref<MaterialBase>> *materials,
										  const aiMesh *aiMesh, const aiScene *scene, const aiMatrix4x4 &transform) {
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

		// Normals are direction vectors, so they need the inverse transpose of the transform to stay
		// perpendicular to the surface when the mesh is scaled non-uniformly.
		aiMatrix3x3 normalMatrix(transform);
		normalMatrix.Inverse().Transpose();

		for (u32 i = 0; i < aiMesh->mNumVertices; i++) {
			// Position, placed in the space of the model as a whole.
			const aiVector3D position = transform * aiMesh->mVertices[i];
			meshData.positions[i] = glm::vec3(position.x, position.y, position.z);

			// Normals
			if (meshData.normals.has_value()) {
				aiVector3D normal = normalMatrix * aiMesh->mNormals[i];
				normal.Normalize();
				meshData.normals.value()[i] = glm::vec3(normal.x, normal.y, normal.z);
			}

			// UV coordinates
			if (meshData.uvs.has_value())
				meshData.uvs.value()[i] = {aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y};
		}

		// The vertices are stored relative to the centroid of the mesh, which becomes the mesh its position.
		const glm::vec3 centroid = FindCentroid(meshData.positions.data(), meshData.positions.size());
		for (glm::vec3 &position: meshData.positions) {
			position -= centroid;
		}

		// Set the indices
		u32 i = 0;
		for (unsigned int x = 0; x < aiMesh->mNumFaces; x++) {
			// ASSUMES THE INDICES ARE 4 BYTE UNSIGNED INTS.
			std::copy_n(aiMesh->mFaces[x].mIndices, aiMesh->mFaces[x].mNumIndices, meshData.indices.begin() + i);
			i += aiMesh->mFaces[x].mNumIndices;
		}

		// Create the mesh entity
		const Ref<MaterialBase>& meshMaterial = (*materials)[aiMesh->mMaterialIndex];

		const entt::entity mesh = target.CreateEntity(aiMesh->mName.length > 0 ? aiMesh->mName.C_Str() : "Mesh");
		target.reg.emplace<MeshComponent>(mesh, std::move(meshData), meshMaterial);

		// Set the position of the mesh
		target.reg.get<TransformComponent>(mesh).Position = centroid;

		return mesh;
	}

	std::vector<Ref<MaterialBase>> ModelLoader::processPhongMaterials(const aiScene *scene,
																	  const std::filesystem::path &directory) {
		std::vector<Ref<MaterialBase>> materials(scene->mNumMaterials);
		std::unordered_map<std::string, Ref<Texture>> textureCache;

		for (int x = 0; x < scene->mNumMaterials; x++) {
			aiMaterial* material = scene->mMaterials[x];
			Ref<PhongMaterial> phongMaterial = createRef<PhongMaterial>();

			// First extract and store the textures
			aiString dif, spec, norm;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &dif);
			material->GetTexture(aiTextureType_SPECULAR, 0, &spec);
			// glTF/PBR sources describe their albedo as a base colour instead of a diffuse map.
			if (dif.length == 0)
				material->GetTexture(aiTextureType_BASE_COLOR, 0, &dif);
			// material->GetTexture(aiTextureType_HEIGHT, 0, &norm);

			if (Ref<Texture> diffuse = loadMaterialTexture(scene, directory, dif, textureCache))
				phongMaterial->AddTexture(PhongMaterialTextureTypes::Diffuse, diffuse);
			if (Ref<Texture> specular = loadMaterialTexture(scene, directory, spec, textureCache))
				phongMaterial->AddTexture(PhongMaterialTextureTypes::Specular, specular);
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
			phongMaterial->Set(PhongMaterialVariables::AmbientFactor, 0.2f);

			// Retrieve diffuse
			if (aiColor3D Diffuse; material->Get(AI_MATKEY_COLOR_DIFFUSE, Diffuse) == aiReturn_SUCCESS)
				phongMaterial->Set(PhongMaterialVariables::Diffuse, Diffuse);
			else
				phongMaterial->Set(PhongMaterialVariables::Diffuse, aiColor3D(1.0f, 1.0f, 1.0f));

			// Retrieve specular
			if (aiColor3D Specular; material->Get(AI_MATKEY_COLOR_SPECULAR, Specular) == aiReturn_SUCCESS)
				phongMaterial->Set(PhongMaterialVariables::Specular, Specular);
			else
				phongMaterial->Set(PhongMaterialVariables::Specular, aiColor3D(1.0f, 1.0f, 1.0f));

			materials[x] = phongMaterial;
		}

		return materials;
	}


	Ref<Texture> ModelLoader::loadMaterialTexture(const aiScene *scene, const std::filesystem::path &directory,
												  const aiString &reference,
												  std::unordered_map<std::string, Ref<Texture>> &cache) {
		if (reference.length == 0)
			return nullptr;

		const std::string key(reference.C_Str());
		if (const auto cached = cache.find(key); cached != cache.end())
			return cached->second;

		Ref<Texture> texture;

		// Self contained formats (.glb, binary .fbx) keep their images inside the model file. Assimp hands
		// those out as a handle like "*0" that does not exist on disk, so they have to be read from the scene.
		if (const aiTexture *embedded = scene->GetEmbeddedTexture(reference.C_Str())) {
			if (embedded->mHeight == 0) {
				// Compressed blob (png/jpg/...), mWidth holds the size in bytes.
				texture = Texture2D::CreateFromMemory(embedded->pcData, embedded->mWidth);
			} else {
				// Raw texels, stored as BGRA8888 and top row first. Both have to be flipped to match the
				// RGBA, bottom row first layout that the file path produces.
				const u32 width = embedded->mWidth;
				const u32 height = embedded->mHeight;

				std::vector<u8> pixels(static_cast<size_t>(width) * height * 4);
				for (u32 y = 0; y < height; y++) {
					const aiTexel *row = embedded->pcData + static_cast<size_t>(y) * width;
					u8 *target = pixels.data() + static_cast<size_t>(height - 1 - y) * width * 4;
					for (u32 x = 0; x < width; x++) {
						target[x * 4 + 0] = row[x].r;
						target[x * 4 + 1] = row[x].g;
						target[x * 4 + 2] = row[x].b;
						target[x * 4 + 3] = row[x].a;
					}
				}

				texture = Texture2D::Create(width, height);
				texture->setData(pixels.data(), static_cast<u32>(pixels.size()));
			}
		} else {
			const std::filesystem::path path = directory / reference.C_Str();
			if (!std::filesystem::exists(path)) {
				CORE_WARN("Texture '{}' referenced by the model does not exist.", path.string());
				return nullptr;
			}

			texture = Texture2D::Create(path.string().c_str());
		}

		cache[key] = texture;
		return texture;
	}

} // Steve