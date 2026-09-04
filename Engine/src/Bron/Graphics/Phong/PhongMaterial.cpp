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

		// Reset the textures
		Set(PhongMaterialVariables::kDiffuseTexture, 0.0f);
		Set(PhongMaterialVariables::kSpecularTexture, 0.0f);
		// Set(PhongMaterialVariables::kNormalTexture, 0);

		// Bind the available textures to texture slots
		int texture_slot_index = texture_slot_starting_index;
		if (textures.contains(PhongMaterialTextureTypes::kDiffuse)) {
			textures.at(PhongMaterialTextureTypes::kDiffuse)->Bind(texture_slot_index);
			Set(PhongMaterialVariables::kDiffuseTexture, static_cast<float>(texture_slot_index));
			texture_slot_index += 1;
		}
		if (textures.contains(PhongMaterialTextureTypes::kSpecular)) {
			textures.at(PhongMaterialTextureTypes::kSpecular)->Bind(texture_slot_index);
			Set(PhongMaterialVariables::kSpecularTexture, static_cast<float>(texture_slot_index));
			texture_slot_index += 1;
		}
		// if (textures.contains(PhongMaterialTextureTypes::kNormal)) {
		// 	textures.at(PhongMaterialTextureTypes::kNormal)->Bind(texture_slot_index);
		// 	Set(PhongMaterialVariables::kNormalTexture, static_cast<float>(texture_slot_index));
		// 	texture_slot_index += 1;
		// }

		shader->SetUniforms(*Layout, Data);

		return texture_slot_index;
	}

	u32 PhongMaterial::NumberUniformCalls() const { return Layout->Data.size(); }

	void PhongMaterial::AddTexture(PhongMaterialTextureTypes texture_type, Ref<Texture> texture) {
	    textures[texture_type] = std::move(texture);
    }
} // namespace bron
