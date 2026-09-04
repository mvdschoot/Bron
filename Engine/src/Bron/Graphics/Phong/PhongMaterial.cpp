//
// Created by mathijs on 8/1/25.
//


#include "PhongMaterial.h"

#include <utility>

namespace bron {
u8 PhongMaterial::Bind(const Ref<Shader> shader, u8 texture_slot_starting_index) {
	if (!shader->IsBound()) {
		shader->Bind();
	}

	// Reset the textures_
	Set(PhongMaterialVariables::kDiffuseTexture, 0.0f);
	Set(PhongMaterialVariables::kSpecularTexture, 0.0f);
	// Set(PhongMaterialVariables::kNormalTexture, 0);

	// Bind the available textures_ to texture slots
	int texture_slot_index = texture_slot_starting_index;
	if (textures_.contains(PhongMaterialTextureTypes::kDiffuse)) {
		textures_.at(PhongMaterialTextureTypes::kDiffuse)->Bind(texture_slot_index);
		Set(PhongMaterialVariables::kDiffuseTexture, static_cast<float>(texture_slot_index));
		texture_slot_index += 1;
	}
	if (textures_.contains(PhongMaterialTextureTypes::kSpecular)) {
		textures_.at(PhongMaterialTextureTypes::kSpecular)->Bind(texture_slot_index);
		Set(PhongMaterialVariables::kSpecularTexture, static_cast<float>(texture_slot_index));
		texture_slot_index += 1;
	}
	// if (textures_.contains(PhongMaterialTextureTypes::kNormal)) {
	// 	textures_.at(PhongMaterialTextureTypes::kNormal)->Bind(texture_slot_index);
	// 	Set(PhongMaterialVariables::kNormalTexture, static_cast<float>(texture_slot_index));
	// 	texture_slot_index += 1;
	// }

	shader->SetUniforms(*layout, data);

	return texture_slot_index;
}

u32 PhongMaterial::NumberUniformCalls() const { return layout->keys.size(); }

void PhongMaterial::AddTexture(PhongMaterialTextureTypes texture_type, Ref<Texture> texture) {
	textures_[texture_type] = std::move(texture);
}
} // namespace bron
