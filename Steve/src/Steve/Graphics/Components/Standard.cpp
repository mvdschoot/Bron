#include "Standard.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

namespace Steve
{
	StandardCubeComponent::StandardCubeComponent(RegistryData* reg)
		: Cube(reg)
	{
		Generate();
		SetColor(glm::vec3(1.0));
	}

	void StandardCubeComponent::Generate()
	{
		float size = 1.0f;
		auto [vertices, normals, indices] =
			GenCubeSmoothVertices(glm::vec3(size));

		// Create material with standard values.
		auto* material = new Material(&StandardMaterialLayout);
		material->Set(MaterialDataTypes::AmbientFactor, 1.0);
		material->Set(MaterialDataTypes::Diffuse, glm::vec3(1.0));
		material->Set(MaterialDataTypes::Specular, glm::vec3(1.0));
		material->Set(MaterialDataTypes::Shininess, 200.0f);
		material->Set(MaterialDataTypes::ShininessStrength, 100.0f);

		auto* context = new MeshContext(StandardInstances::instance().StandardShader.get(), material);

		// Fill vbo with not just vertices, but also normals & texture coords
		usize stride = StandardVertexLayout->GetStride();
		auto* new_verts = new u8[24 * stride];
		usize vec_size = sizeof(glm::vec3);
		for (int x = 0; x < 24 * stride; x += stride)
		{
			memcpy(new_verts + x, vertices + (x / stride), vec_size);
			memcpy(new_verts + x + vec_size, normals + (x / stride), vec_size);
			memset(new_verts + x + (2 * vec_size), 0, sizeof(glm::vec2));
		}

		AddMesh(
			pRegData,
			StandardVertexLayout,
			new_verts, 24 * stride,
			indices, 36,
			context
		)->name = "Mesh 0";
		

		delete[] new_verts;
		delete[] vertices;
	}

	/*
	 * Voor nu support alleen Phong.
	 * Allocates everything on heap
	 */
	MeshContext StandardModelComponent::LoadNewContext(aiMaterial* mat) const
	{
		CH_PROFILE_FUNCTION();

		auto* textures = new TexturePack;

		auto* material = new Material(&StandardMaterialLayout);
		material->Textures = textures;

		aiString dif, spec, norm;
		mat->GetTexture(aiTextureType_DIFFUSE, 0, &dif);
		mat->GetTexture(aiTextureType_SPECULAR, 0, &spec);
		mat->GetTexture(aiTextureType_HEIGHT, 0, &norm);

		if (dif.length != 0)
		{
			textures->Textures[TextureTypes::Diffuse] = Texture2D::Create((Directory + "/" + dif.C_Str()).c_str());
		}
		if (spec.length != 0) {
			textures->Textures[TextureTypes::Specular] = Texture2D::Create((Directory + "/" + spec.C_Str()).c_str());
		}
		if (norm.length != 0) {
			textures->Textures[TextureTypes::Normal] = Texture2D::Create((Directory + "/" + norm.C_Str()).c_str());
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
			material->Set(MaterialDataTypes::Shininess, Shininess);
		else
			material->Set(MaterialDataTypes::Shininess, defaultShininess);

		// Retrieve shininess strength
		if (mat->Get(AI_MATKEY_SHININESS_STRENGTH, ShininessStrength) == aiReturn_SUCCESS)
			material->Set(MaterialDataTypes::ShininessStrength, ShininessStrength);
		else
			material->Set(MaterialDataTypes::ShininessStrength, defaultShininessStrength);

		// Todo: Ambient factor
		material->Set(MaterialDataTypes::AmbientFactor, defaultAmbient);

		// Retrieve diffuse
		if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, Diffuse) == aiReturn_SUCCESS)
			material->Set(MaterialDataTypes::Diffuse, Diffuse);
		else
			material->Set(MaterialDataTypes::Diffuse, defaultDiffuse);

		// Retrieve specular
		if (mat->Get(AI_MATKEY_COLOR_SPECULAR, Specular) == aiReturn_SUCCESS)
			material->Set(MaterialDataTypes::Specular, Specular);
		else
			material->Set(MaterialDataTypes::Specular, defaultSpecular);
		

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

		auto* buffer = new StandardVertexBuffer[mesh->mNumVertices];

		glm::vec3 centroid = Mesh::FindCentroid((glm::vec3*)mesh->mVertices, mesh->mNumVertices);
		
		for (u32 i = 0; i < mesh->mNumVertices; i++)
		{
			StandardVertexBuffer& vertex = buffer[i];

			// Position
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;

			vertex.Position -= centroid;

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

		Mesh* m = AddMesh(
			pRegData,
			StandardVertexLayout,
			buffer, mesh->mNumVertices * sizeof(StandardVertexBuffer),
			indices, num_indices,
			context
		);
		m->name = "Mesh " + std::to_string(MeshCount++);
		m->GetComponent<TransformComponent>()->Position = centroid;
		

		delete[] indices;
		delete[] buffer;
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

		// Set model centroid & adapt Mesh centroids
		glm::vec3 model_centroid = GetCentroid();
		for(Mesh* mesh : Meshes)
		{
			mesh->GetComponent<TransformComponent>()->Position -= model_centroid;
		}
		GetComponent<TransformComponent>()->Position = model_centroid;
	}

	StandardInstances::StandardInstances()
	{
		StandardShader = Shader::CreateShaderFromLocation("../Steve/Assets/Phong3DShader.glsl");
	}
}
