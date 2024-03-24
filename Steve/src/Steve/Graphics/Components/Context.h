#ifndef __CONTEXT_JEADRE__
#define __CONTEXT_JEADRE__

#include "Steve/Core/Core.h"

#include "Steve/Graphics/Texture.h"
#include "Steve/Graphics/Shader.h"

#include "Uniforms.h"

#include <map>

namespace Steve
{
	enum class MaterialDataTypes
	{
		AmbientFactor,
		Diffuse,
		Specular,
		Shininess,
		ShininessStrength,

		// The following are floats/integers to the texture slot
		DiffuseTexture,
		SpecularTexture,
		NormalTexture
	};

	enum class ShadingTech
	{
		PHONG,
		PBR
	};

	enum class TextureTypes
	{
		Diffuse,
		Specular,
		Normal
	};

	struct TexturePack
	{
		std::map<TextureTypes, Ref<Texture>> Textures;
	};

	inline static MaterialDataTypes TextureConverter(TextureTypes type)
	{
		if (type == TextureTypes::Diffuse) return MaterialDataTypes::DiffuseTexture;
		if (type == TextureTypes::Specular) return MaterialDataTypes::SpecularTexture;
		if (type == TextureTypes::Normal) return MaterialDataTypes::NormalTexture;
		CORE_ASSERT(false, "Texture not here mate")
	}
	inline static TextureTypes TextureConverter(MaterialDataTypes type)
	{
		if (type == MaterialDataTypes::DiffuseTexture) return TextureTypes::Diffuse;
		if (type == MaterialDataTypes::SpecularTexture) return TextureTypes::Specular;
		if (type == MaterialDataTypes::NormalTexture) return TextureTypes::Normal;
		CORE_ASSERT(false, "Texture not here mate")
	}

	struct Material : public UniformData<MaterialDataTypes>
	{
		TexturePack* Textures;
	};

	class MeshContext
	{
	public:
		MeshContext(Shader* shader, Material* material)
			: pMaterial(material), pShader(shader) {}

		~MeshContext() = default;

		Material* pMaterial;
		Shader* pShader;
	};
}

#endif