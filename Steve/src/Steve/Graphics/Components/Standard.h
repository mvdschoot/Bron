#ifndef __STANDARD_RENDERING_FILE__
#define __STANDARD_RENDERING_FILE__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include <assimp/material.h>
#include <assimp/scene.h>

#include "RenderComponents.h"

namespace Steve::graphics {

	class StandardCubeComponent : public Cube
	{
	public:
		StandardCubeComponent(RegistryData* reg);

	private:
		void Generate();
	};

	struct StandardModelComponent : public Model
	{
	private:
		std::map<u32, MeshContext*> RenderContexts;
		std::string Directory;

		MeshContext LoadNewContext(aiMaterial* mat) const;
		void ProcessMesh(aiMesh* mesh, const aiScene* scene);
		void ProcessNode(aiNode* node, const aiScene* scene);

	public:
		StandardModelComponent(RegistryData* reg, const char* model_location);
	};

	struct StandardVertexBuffer
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TextureCoordinates;
	};

	struct StandardMaterialBuffer
	{
		float AmbientFactor;
		glm::vec3 Diffuse;
		glm::vec3 Specular;
		float Shininess;
		float ShininessStrength;
		float DiffuseTexture;
		float SpecularTexture;
		float NormalTexture;
	};

	inline BufferLayout* StandardVertexLayout = new BufferLayout({
				{"a_Position", ShaderDataType::Float3},
				{"a_Normal", ShaderDataType::Float3},
				{"a_TexCoords", ShaderDataType::Float2}
		});

	inline const UniformLayout<MaterialDataTypes>& StandardMaterialLayout = *(new UniformLayout<MaterialDataTypes>({
		{
			{MaterialDataTypes::AmbientFactor, {"uMaterial.AmbientFactor", ShaderDataType::Float}},
			{MaterialDataTypes::Diffuse, {"uMaterial.Diffuse", ShaderDataType::Float3}},
			{MaterialDataTypes::Specular, {"uMaterial.Specular", ShaderDataType::Float3}},
			{MaterialDataTypes::Shininess, {"uMaterial.Shininess", ShaderDataType::Float}},
			{MaterialDataTypes::ShininessStrength, {"uMaterial.ShininessStrength", ShaderDataType::Float}},
			{MaterialDataTypes::DiffuseTexture, {"uMaterial.DiffuseTexture", ShaderDataType::Float}},
			{MaterialDataTypes::SpecularTexture, {"uMaterial.SpecularTexture", ShaderDataType::Float}},
			{MaterialDataTypes::NormalTexture, {"uMaterial.NormalTexture", ShaderDataType::Float}}
		} }));

	inline const UniformLayout<LightDataTypes>& StandardPointLightLayout = *(new UniformLayout<LightDataTypes>({
			{LightDataTypes::Position, {"uLight[].Position", ShaderDataType::Float3}},
			{LightDataTypes::Color, {"uLight[].Color", ShaderDataType::Float3}}
		}));


	class StandardInstances {
	private:
		StandardInstances(); // Disallow instantiation outside of the class.
	public:
		StandardInstances(const StandardInstances&) = delete;
		StandardInstances& operator=(const StandardInstances&) = delete;
		StandardInstances(StandardInstances&&) = delete;
		StandardInstances& operator=(StandardInstances&&) = delete;

		static auto& instance() {
			static StandardInstances test;
			return test;
		}

		Ref<Shader> StandardShader;

	};
}

#endif //__STANDARD_RENDERING_FILE__