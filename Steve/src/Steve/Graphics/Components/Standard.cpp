#include "Standard.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

namespace Steve::graphics
{
	StandardCubeComponent::StandardCubeComponent(RegistryData* reg, const glm::vec3 pos, const glm::vec3 dim)
		: Model(reg)
	{
		Set(pos, dim);
		SetColor(glm::vec3(1.0));
	}

	void StandardCubeComponent::Set(const glm::vec3 pos, const glm::vec3 dim)
	{
		mPosition = pos;
		mDimensions = dim;

		StandardMaterialBuffer* materialBuffer = new StandardMaterialBuffer({
			0.2,
			glm::vec3(1.0),
			glm::vec3(1.0),
			1.0,
			1.0
		});

		auto* material = new Material();
		material->Data = (u8*)materialBuffer;
		material->Layout = &StandardMaterialLayout;
		material->Textures = nullptr;

		auto* context = new MeshContext(StandardInstances::instance().StandardShader.get(), material);

		auto [vertices, normals, indices] = GenCubeSmoothVertices(pos, dim);

		// Fill vbo with not just vertices, but also normals & texture coords
		usize size = material->Layout->GetStride();
		auto* new_verts = new u8[size];
		usize vec_size = sizeof(glm::vec3);
		for (int x = 0; x < 24 * size; x += size)
		{
			memcpy_s(new_verts + x, vec_size, vertices + (x / size), vec_size);
			memcpy_s(new_verts + x + vec_size, vec_size, normals + (x / size), vec_size);
			memset(new_verts + x + (2 * vec_size), 0, sizeof(glm::vec2));
		}

		Meshes.emplace_back(
			*StandardVertexLayout,
			new_verts, 24 * sizeof(glm::vec3),
			indices, 36,
			context
		);

		delete[] new_verts;
		delete[] vertices;
	}

	void StandardCubeComponent::SetColor(glm::vec3 color)
	{
		Meshes[0].pContext->pMaterial->Set(ColorType::Diffuse, (u8*)&color);
		Meshes[0].pContext->pMaterial->Set(ColorType::Specular, (u8*)&color);
	}

	/*
	 * Voor nu support alleen Phong.
	 * Allocates everything on heap
	 */
	MeshContext StandardModelComponent::LoadNewContext(aiMaterial* mat) const
	{
		CH_PROFILE_FUNCTION();

		auto* textures = new TexturePack;

		auto* material = new Material;
		material->Layout = &StandardMaterialLayout;
		material->Data = (u8*)new StandardMaterialBuffer;
		material->Textures = textures;

		aiString dif, spec, norm;
		mat->GetTexture(aiTextureType_DIFFUSE, 0, &dif);
		mat->GetTexture(aiTextureType_SPECULAR, 0, &spec);
		mat->GetTexture(aiTextureType_HEIGHT, 0, &norm);

		if (dif.length != 0)
		{
			textures->Textures[TextureType::DIFFUSE] = Texture2D::Create((Directory + "/" + dif.C_Str()).c_str());
		}
		if (spec.length != 0) {
			textures->Textures[TextureType::SPECULAR] = Texture2D::Create((Directory + "/" + spec.C_Str()).c_str());
		}
		if (norm.length != 0) {
			textures->Textures[TextureType::NORMAL] = Texture2D::Create((Directory + "/" + norm.C_Str()).c_str());
		}

		float defaultAmbient = 0.2;
		float defaultShininess = 5.0f;
		float defaultShininessStrength = 1.0f;
		aiColor3D defaultDiffuse(1.0f);
		aiColor3D defaultSpecular(1.0f);
		float AmbientFactor;
		float Shininess;
		float ShininessStrength;
		aiColor3D Diffuse;
		aiColor3D Specular;

		// Retrieve shininess
		if (mat->Get(AI_MATKEY_SHININESS, Shininess) == aiReturn_SUCCESS)
			material->Set(ColorType::Shininess, (u8*)&Shininess);
		else
			material->Set(ColorType::Shininess, (u8*)&defaultShininess);

		// Retrieve shininess strength
		if (mat->Get(AI_MATKEY_SHININESS_STRENGTH, ShininessStrength) == aiReturn_SUCCESS)
			material->Set(ColorType::ShininessStrength, (u8*)&ShininessStrength);
		else
			material->Set(ColorType::ShininessStrength, (u8*)&defaultShininessStrength);

		// Todo: Ambient factor
		material->Set(ColorType::AmbientFactor, (u8*)&defaultAmbient);

		// Retrieve diffuse
		if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, Diffuse) == aiReturn_SUCCESS)
			material->Set(ColorType::Diffuse, (u8*)&Diffuse);
		else
			material->Set(ColorType::Diffuse, (u8*)&defaultDiffuse);

		// Retrieve specular
		if (mat->Get(AI_MATKEY_COLOR_SPECULAR, Specular) == aiReturn_SUCCESS)
			material->Set(ColorType::Specular, (u8*)&Specular);
		else
			material->Set(ColorType::Specular, (u8*)&defaultSpecular);
		

		return { StandardInstances::instance().StandardShader.get(), material };
	}

	void StandardModelComponent::ProcessMesh(aiMesh* mesh, const aiScene* scene)

	{
		CH_PROFILE_FUNCTION();

		u32 num_indices = 0;
		for (u32 x = 0; x < mesh->mNumFaces; x++)
		{
			num_indices += mesh->mFaces[x].mNumIndices;
		}

		auto* vertices = new StandardVertexBuffer[mesh->mNumVertices];
		for (u32 i = 0; i < mesh->mNumVertices; i++)
		{
			StandardVertexBuffer& vertex = vertices[i];
			glm::vec3 vector;

			// Position
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;

			// Normals
			if (mesh->mNormals == nullptr) {
				vertex.Normal = glm::vec3(1.0f);
			}
			else {
				vertex.Normal.x = mesh->mNormals[i].x;
				vertex.Normal.y = mesh->mNormals[i].y;
				vertex.Normal.z = mesh->mNormals[i].z;
			}

			// UV coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
				vertex.TextureCoordinates = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			else
				vertex.TextureCoordinates = glm::vec2(0.0f, 0.0f);

		}

		auto* indices = new u32[num_indices];

		u32 i = 0;
		for (unsigned int x = 0; x < mesh->mNumFaces; x++)
		{
			std::copy_n(mesh->mFaces[x].mIndices, mesh->mFaces[x].mNumIndices, indices + i);
			i += mesh->mFaces[x].mNumIndices;
		}


		MeshContext* context;
		if (RenderContexts.contains(mesh->mMaterialIndex))
		{
			context = RenderContexts[mesh->mMaterialIndex];
		}
		else
		{
			// Create new material
			// Insert context with new material and existing shader into map
			// Set local function context to pointer to map element
			MeshContext* c = new MeshContext(LoadNewContext(scene->mMaterials[mesh->mMaterialIndex]));
			RenderContexts.emplace(mesh->mMaterialIndex, c);
			context = c;
		}

		Meshes.emplace_back(
			*StandardVertexLayout,
			vertices, mesh->mNumVertices * sizeof(StandardVertexBuffer),
			indices, num_indices,
			context
		);

		delete[] indices;
		delete[] vertices;
	}

	void StandardModelComponent::ProcessNode(aiNode* node, const aiScene* scene)
	{
		CH_PROFILE_FUNCTION();
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessMesh(mesh, scene);
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	StandardModelComponent::StandardModelComponent(RegistryData* reg, const char* model_location)
		: Model(reg), RenderContexts({})
	{
		CH_PROFILE_FUNCTION();

		// Assimp load model
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(model_location, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

		std::string error = "ERROR::ASSIMP::";
		error.append(importer.GetErrorString());
		CORE_ASSERT(scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode, error)

		std::string loc = model_location;
		Directory = loc.substr(0, loc.find_last_of('/'));

		ProcessNode(scene->mRootNode, scene);
	}

	StandardInstances::StandardInstances()
	{
		StandardShader = Shader::CreateShaderFromLocation("../../../Steve/Assets/Phong3DShader.glsl");
	}
}
