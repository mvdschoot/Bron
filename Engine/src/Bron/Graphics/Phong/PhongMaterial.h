//
// Created by mathijs on 8/1/25.
//

#pragma once
#include "Bron/Graphics/MaterialBase.h"
#include "Bron/Graphics/ShaderRegistry.h"
#include "Bron/Graphics/Texture.h"
#include "Bron/Graphics/Components/BufferExtentions.h"

namespace bron {
    enum class PhongMaterialTextureTypes
    {
        Diffuse,
        Specular,
        // Normal
    };

    // These are the variables that the material has.
    enum class PhongMaterialVariables
    {
        AmbientFactor,
        Diffuse,
        Specular,
        Shininess,
        ShininessStrength,

        // The following are floats/integers to the texture slot
        DiffuseTexture,
        SpecularTexture,
        // NormalTexture
    };

	inline const auto PhongMaterialLayout = NamedBufferLayout<PhongMaterialVariables>({
        {PhongMaterialVariables::AmbientFactor, {"u_Material.AmbientFactor", ShaderDataType::Float}},
        {PhongMaterialVariables::Diffuse, {"u_Material.Diffuse", ShaderDataType::Float3}},
        {PhongMaterialVariables::Specular, {"u_Material.Specular", ShaderDataType::Float3}},
        {PhongMaterialVariables::Shininess, {"u_Material.Shininess", ShaderDataType::Float}},
        {PhongMaterialVariables::ShininessStrength, {"u_Material.ShininessStrength", ShaderDataType::Float}},
        {PhongMaterialVariables::DiffuseTexture, {"u_Material.DiffuseTexture", ShaderDataType::Float}},
        {PhongMaterialVariables::SpecularTexture, {"u_Material.SpecularTexture", ShaderDataType::Float}},
        // {PhongMaterialVariables::NormalTexture, {"u_Material.NormalTexture", ShaderDataType::Float}}
    });


    class PhongMaterial final : public MaterialBase, public NamedBufferData<PhongMaterialVariables> {
    public:
		PhongMaterial() :
			MaterialBase(MaterialWorkflow::PHONG, PHONG_SHADER),
    		NamedBufferData<PhongMaterialVariables>(&PhongMaterialLayout) {}

		u8 Bind(Ref<Shader> shader, u8 textureSlotStartingIndex) override;
    	[[nodiscard]] u32 NumberUniformCalls() const override;
    	void AddTexture(PhongMaterialTextureTypes textureType, Ref<Texture> texture);

	private:
        std::map<PhongMaterialTextureTypes, Ref<Texture>> textures;
    };
}
