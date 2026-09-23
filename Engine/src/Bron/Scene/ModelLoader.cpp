//
// Created by mathijs on 8/2/25.
//

#include "ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <algorithm>

#include "Bron/Core/Logger.h"
#include "Bron/Core/Profiling.h"

namespace bron {

std::optional<ImportedModel> ModelLoader::Import(const std::filesystem::path& location) {
	BR_PROFILE_FUNCTION();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(location.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode == nullptr) {
		BR_CORE_ERROR("Could not import {}: {}", location.string(), importer.GetErrorString());
		return std::nullopt;
	}

	// Textures are referenced relative to the model file.
	const std::filesystem::path directory = location.parent_path();

	ImportedModel model;

	model.materials.reserve(scene->mNumMaterials);
	for (u32 i = 0; i < scene->mNumMaterials; i++)
		model.materials.push_back(ProcessMaterial(scene, scene->mMaterials[i], directory, model));

	model.meshes.reserve(scene->mNumMeshes);
	for (u32 i = 0; i < scene->mNumMeshes; i++)
		model.meshes.push_back(ProcessMesh(scene->mMeshes[i]));

	ProcessNode(scene->mRootNode, std::nullopt, model);

	// The root's name is whatever the exporter wrote ("RootNode", "Scene", ...); the file
	// name is what a user will recognise.
	model.nodes[0].name = location.stem().string();

	return model;
}

void ModelLoader::ProcessNode(const aiNode* node, const std::optional<u32> parent, ImportedModel& model) {
	const u32 index = static_cast<u32>(model.nodes.size());

	ImportedNode& imported = model.nodes.emplace_back();
	imported.name = node->mName.length > 0 ? node->mName.C_Str() : "Node";
	imported.parent = parent;

	// Kept as a local transform rather than baked into the vertices: formats that keep
	// their scene graph (glTF especially) place one mesh several times this way, and
	// every placement shares the one mesh asset.
	aiVector3D scaling, position;
	aiQuaternion rotation;
	node->mTransformation.Decompose(scaling, rotation, position);
	imported.position = {position.x, position.y, position.z};
	imported.rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
	imported.scale = {scaling.x, scaling.y, scaling.z};

	imported.meshes.assign(node->mMeshes, node->mMeshes + node->mNumMeshes);

	// 'imported' is not used past this point: the recursion grows the vector and may move it.
	for (u32 i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], index, model);
}

ImportedMesh ModelLoader::ProcessMesh(const aiMesh* ai_mesh) {
	BR_PROFILE_FUNCTION();

	ImportedMesh imported;
	imported.name = ai_mesh->mName.length > 0 ? ai_mesh->mName.C_Str() : "Mesh";
	imported.material = ai_mesh->mMaterialIndex;

	MeshData& data = imported.data;
	const u32 vertices = ai_mesh->mNumVertices;

	// The vertices are kept exactly as the file stores them, in the mesh's own space.
	data.positions.resize(vertices);
	for (u32 i = 0; i < vertices; i++)
		data.positions[i] = {ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z};

	if (ai_mesh->HasNormals()) {
		data.normals = std::vector<glm::vec3>(vertices);
		for (u32 i = 0; i < vertices; i++)
			data.normals.value()[i] = {ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z};
	}

	if (ai_mesh->HasTangentsAndBitangents()) {
		data.tangents = std::vector<glm::vec3>(vertices);
		for (u32 i = 0; i < vertices; i++)
			data.tangents.value()[i] = {ai_mesh->mTangents[i].x, ai_mesh->mTangents[i].y, ai_mesh->mTangents[i].z};
	}

	if (ai_mesh->HasTextureCoords(0)) {
		data.uvs = std::vector<glm::vec2>(vertices);
		for (u32 i = 0; i < vertices; i++)
			data.uvs.value()[i] = {ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y};
	}

	// aiProcess_Triangulate leaves only triangles - and the odd point or line, which
	// have fewer indices and are copied as they are.
	for (u32 face = 0; face < ai_mesh->mNumFaces; face++) {
		const aiFace& f = ai_mesh->mFaces[face];
		data.indices.insert(data.indices.end(), f.mIndices, f.mIndices + f.mNumIndices);
	}

	return imported;
}

ImportedMaterial ModelLoader::ProcessMaterial(const aiScene* scene, const aiMaterial* ai_material,
											  const std::filesystem::path& directory, ImportedModel& model) {
	ImportedMaterial imported;

	aiString name;
	if (ai_material->Get(AI_MATKEY_NAME, name) == aiReturn_SUCCESS)
		imported.name = name.C_Str();

	aiString diffuse, specular;
	ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse);
	ai_material->GetTexture(aiTextureType_SPECULAR, 0, &specular);
	// glTF/PBR sources describe their albedo as a base colour instead of a diffuse map.
	if (diffuse.length == 0)
		ai_material->GetTexture(aiTextureType_BASE_COLOR, 0, &diffuse);

	imported.diffuse_texture = ProcessTexture(scene, directory, diffuse, model);
	imported.specular_texture = ProcessTexture(scene, directory, specular, model);

	// Anything the file leaves out keeps the default from ImportedMaterial.
	if (float shininess; ai_material->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS)
		imported.shininess = shininess;

	if (float strength; ai_material->Get(AI_MATKEY_SHININESS_STRENGTH, strength) == aiReturn_SUCCESS)
		imported.shininess_strength = strength;

	if (aiColor3D color; ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS)
		imported.diffuse = {color.r, color.g, color.b};

	if (aiColor3D color; ai_material->Get(AI_MATKEY_COLOR_SPECULAR, color) == aiReturn_SUCCESS)
		imported.specular = {color.r, color.g, color.b};

	return imported;
}

std::optional<u32> ModelLoader::ProcessTexture(const aiScene* scene, const std::filesystem::path& directory,
											   const aiString& reference, ImportedModel& model) {
	if (reference.length == 0)
		return std::nullopt;

	const std::string key(reference.C_Str());
	const auto seen = std::ranges::find(model.textures, key, &ImportedTexture::reference);
	if (seen != model.textures.end())
		return static_cast<u32>(seen - model.textures.begin());

	ImportedTexture texture;
	texture.reference = key;

	// Self contained formats keep their images inside the model file. Assimp hands those
	// out as a handle like "*0" that does not exist on disk, so they are read from the scene.
	if (const aiTexture* embedded = scene->GetEmbeddedTexture(reference.C_Str())) {
		texture.embedded = true;

		if (embedded->mHeight == 0) {
			// Compressed blob, mWidth holds the size in bytes.
			const u8* bytes = reinterpret_cast<const u8*>(embedded->pcData);
			texture.bytes.assign(bytes, bytes + embedded->mWidth);
		} else {
			// Raw texels, top row first. Flipped to match the bottom row first layout that
			// a texture loaded from a file ends up in.
			texture.width = embedded->mWidth;
			texture.height = embedded->mHeight;
			texture.bytes.resize(static_cast<size_t>(texture.width) * texture.height * 4);

			for (u32 y = 0; y < texture.height; y++) {
				const aiTexel* row = embedded->pcData + static_cast<size_t>(y) * texture.width;
				u8* target = texture.bytes.data() + static_cast<size_t>(texture.height - 1 - y) * texture.width * 4;
				for (u32 x = 0; x < texture.width; x++) {
					target[x * 4 + 0] = row[x].r;
					target[x * 4 + 1] = row[x].g;
					target[x * 4 + 2] = row[x].b;
					target[x * 4 + 3] = row[x].a;
				}
			}
		}
	} else {
		texture.path = (directory / reference.C_Str()).lexically_normal();
		if (!std::filesystem::exists(texture.path)) {
			BR_CORE_WARN("Texture '{}' referenced by the model does not exist.", texture.path.string());
			return std::nullopt;
		}
	}

	model.textures.push_back(std::move(texture));
	return static_cast<u32>(model.textures.size() - 1);
}

} // namespace bron
