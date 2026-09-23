//
// Created by mathijs on 8/1/25.
//

#pragma once
#include "Bron/Graphics/MaterialBase.h"
#include "Bron/Graphics/ShaderRegistry.h"
#include "Bron/Graphics/Texture.h"
#include "Bron/Graphics/Components/BufferExtentions.h"

namespace bron {

// These are the variables that the material has.
enum class PhongMaterialVariables {
	kAmbientFactor,
	kDiffuse,
	kSpecular,
	kShininess,
	kShininessStrength,

	// The following are floats/integers to the texture slot
	kDiffuseTexture,
	kSpecularTexture,
	// kNormalTexture
};

inline const auto kPhongMaterialLayout = NamedBufferLayout<PhongMaterialVariables>({
		{PhongMaterialVariables::kAmbientFactor, {"u_Material.AmbientFactor", ShaderDataType::kFloat}},
		{PhongMaterialVariables::kDiffuse, {"u_Material.Diffuse", ShaderDataType::kFloat3}},
		{PhongMaterialVariables::kSpecular, {"u_Material.Specular", ShaderDataType::kFloat3}},
		{PhongMaterialVariables::kShininess, {"u_Material.Shininess", ShaderDataType::kFloat}},
		{PhongMaterialVariables::kShininessStrength, {"u_Material.ShininessStrength", ShaderDataType::kFloat}},
		{PhongMaterialVariables::kDiffuseTexture, {"u_Material.DiffuseTexture", ShaderDataType::kFloat}},
		{PhongMaterialVariables::kSpecularTexture, {"u_Material.SpecularTexture", ShaderDataType::kFloat}},
		// {PhongMaterialVariables::kNormalTexture, {"u_Material.NormalTexture", ShaderDataType::kFloat}}
});


class PhongMaterial final : public MaterialBase, public NamedBufferData<PhongMaterialVariables> {
public:
	PhongMaterial() :
		MaterialBase(MaterialWorkflow::kPhong, BR_PHONG_SHADER),
		NamedBufferData<PhongMaterialVariables>(&kPhongMaterialLayout) {}

	u8 Bind(Ref<Shader> shader, u8 texture_slot_starting_index) override;
	[[nodiscard]] u32 NumberUniformCalls() const override;
	void AddTexture(TextureType texture_type, Ref<Texture> texture);

private:
	std::map<TextureType, Ref<Texture>> textures_;
};
} // namespace bron
